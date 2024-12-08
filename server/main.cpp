#include <fstream>
#include <iostream>

#include "intro/json.hpp"

#include "log.hpp"
#include "order_book.hpp"
#include "server.hpp"

// Function to accept incoming WebSocket connections
void do_accept(
    boost::asio::io_context &ioc,
    boost::asio::ip::tcp::acceptor &acceptor,
    OrderBook &orderBook)
{
    acceptor.async_accept(
        [&](boost::beast::error_code ec, boost::asio::ip::tcp::socket socket) {
            if (!ec) {
                std::make_shared<WebSocketSession>(std::move(socket), orderBook)
                    ->start();
            }

            // Accept the next connection
            do_accept(ioc, acceptor, orderBook);
        });
}

// Simulate adding orders to the order book (for demonstration)
void initOrderBook(OrderBook &orderBook, const std::string filename)
{

    auto istrmMarketFeed = std::ifstream{filename, std::ios::in};

    if (!istrmMarketFeed.is_open()) {
        LOG_ERROR("Cannot open file ", filename);
        std::exit(1);
    }

    for (std::string line; std::getline(istrmMarketFeed, line);) {
        auto order = decodeJSON<Order>(line);

        orderBook.processOrder(order);
    }
}

int main(int argc, const char *argv[])
{
    try {
        // Set up the I/O context and acceptor
        boost::asio::io_context ioc{1};
        boost::asio::ip::tcp::acceptor acceptor{
            ioc, {boost::asio::ip::tcp::v4(), 9002}};

        OrderBook orderBook;

        initOrderBook(orderBook, argv[1]);

        // Start the acceptor
        do_accept(ioc, acceptor, orderBook);

        // Run the I/O context
        ioc.run();
    }
    catch (const std::exception &e) {
        LOG_ERROR("Exception: ", e.what());
    }

    return 0;
}