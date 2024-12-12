#pragma once

#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/steady_timer.hpp>
#include <boost/asio/strand.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/core/detail/base64.hpp>
#include <boost/beast/websocket.hpp>

#include "intro/json.hpp"
#include "log.hpp"
#include "order_book.hpp"

// WebSocket session that will handle each client
struct WebSocketSession : public std::enable_shared_from_this<WebSocketSession>
{
    WebSocketSession(
        boost::asio::ip::tcp::socket &&socket, OrderBook &orderBook) :
        ws_(std::move(socket)),
        orderBook_(orderBook)
    {}

    void start()
    {
        // Perform the WebSocket handshake
        ws_.async_accept(boost::beast::bind_front_handler(
            &WebSocketSession::on_accept, shared_from_this()));
    }

    void sendOrderBook()
    {
        nlohmann::json bids(orderBook_.bids_);
        nlohmann::json asks(orderBook_.asks_);
        nlohmann::json trades(orderBook_.trades_.trades);

        nlohmann::json orderBookJson;

        orderBookJson["bids"] = bids;
        orderBookJson["asks"] = asks;
        orderBookJson["trades"] = trades;

        ws_.async_write(
            boost::asio::buffer(orderBookJson.dump()),
            boost::beast::bind_front_handler(
                &WebSocketSession::on_send, shared_from_this()));
    }

    // Process the order placed by the client
    void on_message(boost::beast::error_code ec, std::size_t bytes_transferred)
    {
        if (ec) {
            LOG_ERROR("Error receiving message: ", ec.message());

            return;
        }

        // Extract the raw data from the multi_buffer
        std::string message;
        for (const auto &buffer_segment : buffer_.data()) {
            // Append each segment (buffer) to the message string
            message.append(
                boost::asio::buffer_cast<const char *>(buffer_segment),
                boost::asio::buffer_size(buffer_segment));
        }

        LOG_INFO("Recived from client: ", message);

        // Parse the incoming JSON message
        try {
            auto order = decodeJSON<Order>(message);

            // Process the order (add to the order book)
            orderBook_.processOrder(order);

            // Send the updated order book to all clients
            sendOrderBook();
        }
        catch (const std::exception &e) {
            LOG_ERROR("Error processing order: ", e.what());
        }

        // Clear the buffer to prepare for the next message
        buffer_.consume(bytes_transferred);

        ws_.async_read(
            buffer_,
            boost::beast::bind_front_handler(
                &WebSocketSession::on_message, shared_from_this()));
    }

private:
    void on_accept(boost::beast::error_code ec)
    {
        if (ec) {
            LOG_ERROR("Accept failed: ", ec.message());

            return;
        }

        LOG_INFO("WebSocket connection established.");

        sendOrderBook();

        // Start listening for messages (orders)
        ws_.async_read(
            buffer_,
            boost::beast::bind_front_handler(
                &WebSocketSession::on_message, shared_from_this()));
    }

    void on_send(boost::beast::error_code ec, std::size_t bytes_transferred)
    {
        if (ec) {
            LOG_ERROR("Send failed: ", ec.message());

            return;
        }

        LOG_INFO("Sent ", bytes_transferred, " bytes to the client.");
    }

    boost::beast::multi_buffer buffer_;
    boost::beast::websocket::stream<boost::asio::ip::tcp::socket> ws_;
    OrderBook &orderBook_;
};