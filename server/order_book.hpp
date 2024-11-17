#pragma once

#include <chrono>
#include <mutex>
#include <set>
#include <vector>

#include "order.hpp"

struct TradingHistroy
{
    void addTrade(const Trade &trade)
    {
        std::lock_guard<std::mutex> lock(mutex_);

        trades.push_back(trade);
    }

    std::vector<Trade> trades;
    mutable std::mutex mutex_;
};

class OrderBook
{
public:
    void addOrder(Order &order)
    {
        std::lock_guard<std::mutex> lock(mutex_);

        if (order.type == Type::Market) {
            // Directly match the order
            tryToMatch(order);
        }
        else {
            // Try to match and then add to order book if unfulfilled
            tryToMatch(order);

            if (order.quantity > 0) {
                if (order.side == Side::Buy) {
                    bids_.insert(order);
                }
                else {
                    asks_.insert(order);
                }
            }
        }
    }

    void tryToMatch(Order &order)
    {
        if (order.side == Side::Buy) {
            matchOrder(bids_, asks_, std::greater_equal{}, order);
        }
        else {
            matchOrder(asks_, bids_, std::less_equal{}, order);
        }
    }

    template<typename Side_, typename OtherSide_, typename Compare>
    void
    matchOrder(Side_ &side, OtherSide_ &otherSide, Compare cmp, Order &order)
    {
        auto it = otherSide.begin();
        while (it != otherSide.end() && order.quantity > 0) {
            if (order.type == Type::Market || cmp(order.price, it->price)) {
                // Match with an ask order
                auto tradedQty = std::min(order.quantity, it->quantity);

                auto tradeTimestamp =
                    std::chrono::duration_cast<std::chrono::nanoseconds>(
                        std::chrono::system_clock::now().time_since_epoch())
                        .count();

                auto trade = Trade{
                    .price = it->price,
                    .quantity = tradedQty,
                    .timestamp = tradeTimestamp};

                trades_.addTrade(trade);

                order.quantity -= tradedQty;
                auto otherSideOrder = *it;
                otherSide.erase(it);
                otherSideOrder.quantity -= tradedQty;

                if (otherSideOrder.quantity > 0) {
                    otherSide.insert(otherSideOrder);
                }

                it = otherSide.begin();
            }
            else {
                break;
            }
        }
    }

    // void matchOrder(Order & order)
    // {
    //     if (order.side == Side::Buy) {
    //         auto it = asks_.begin();
    //         while (it != asks_.end() && order.quantity > 0) {
    //             if (order.type == Type::Market ||
    //                 order.price >= it->price) {
    //                 // Match with an ask order
    //                 auto tradedQty = std::min(order.quantity,
    //                 it->quantity);
    //                 // std::cout << "Matched BUY order " << order.id
    //                 //           << " with SELL order " << it->id
    //                 //           << " for quantity " << tradedQty << " at
    //                 //           price " << it->price << "\n";

    //                 auto tradeTimestamp =
    //                     std::chrono::duration_cast<std::chrono::seconds>(
    //                         std::chrono::system_clock::now()
    //                             .time_since_epoch())
    //                         .count();

    //                 auto trade = Trade{
    //                     .price = it->price,
    //                     .quantity = tradedQty,
    //                     .timestamp = tradeTimestamp};

    //                 trades_.addTrade(trade);

    //                 std::cout << trade << std::endl;

    //                 order.quantity -= tradedQty;
    //                 auto ask = *it;
    //                 asks_.erase(it);
    //                 ask.quantity -= tradedQty;
    //                 if (ask.quantity > 0) {
    //                     asks_.insert(ask);
    //                 }
    //                 it = asks_.begin();
    //             }
    //             else {
    //                 break;
    //             }
    //         }
    //     }
    //     else {  // SELL
    //         auto it = bids_.begin();
    //         while (it != bids_.end() && order.quantity > 0) {
    //             if (order.type == Type::Market ||
    //                 order.price <= it->price) {
    //                 // Match with a bid order
    //                 int tradedQty = std::min(order.quantity,
    //                 it->quantity);
    //                 // std::cout << "Matched SELL order " << order.id
    //                 //           << " with BUY order " << it->id
    //                 //           << " for quantity " << tradedQty << " at
    //                 //           price " << it->price << "\n";

    //                 order.quantity -= tradedQty;
    //                 auto bid = *it;
    //                 bids_.erase(it);
    //                 bid.quantity -= tradedQty;
    //                 if (bid.quantity > 0) {
    //                     bids_.insert(bid);
    //                 }
    //                 it = bids_.begin();
    //             }
    //             else {
    //                 break;
    //             }
    //         }
    //     }
    // }

    void processOrder(Order &order)
    {
        // For simplicity, just add the order to the order book
        addOrder(order);
    }

public:
    std::set<Order, std::greater<Order>> bids_;
    std::set<Order, std::less<Order>> asks_;
    TradingHistroy trades_;
    mutable std::mutex mutex_;
};
