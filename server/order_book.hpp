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
    void processOrder(Order &order)
    {
        std::lock_guard<std::mutex> lock(mutex_);

        // Directly match the order (Market only)
        tryToMatch(order);

        // Add to order book if unfulfilled (Limit only)
        if (order.type == Type::Limit && order.quantity > 0) {
            if (order.side == Side::Buy) {
                bids_.insert(order);
            }
            else {
                asks_.insert(order);
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
                // Match with an other side order
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

public:
    std::set<Order, std::greater<Order>> bids_;
    std::set<Order, std::less<Order>> asks_;
    TradingHistroy trades_;
    mutable std::mutex mutex_;
};
