#pragma once

#include "intro/metadata.hpp"

#include <cstdint>

using Price = std::uint32_t;
using Quantity = std::uint32_t;
using OrderType = std::uint8_t;
using OrderSide = std::uint8_t;

enum Type : std::uint8_t
{
    Market = 0,
    Limit
};

enum Side : std::uint8_t
{
    Buy,
    Sell,
};

struct Order
{
    Price price;
    Quantity quantity;
    OrderType type;
    OrderSide side;

    GEN_METADATA(price, quantity, type, side);
};

bool operator<(const Order &lhs, const Order &rhs)
{
    return lhs.price < rhs.price;
}

bool operator>(const Order &lhs, const Order &rhs)
{
    return lhs.price > rhs.price;
}

namespace metadata {
template<>
struct NameOf<Order>
{
    static const char *get()
    {
        return "Order";
    }
};
}  // namespace metadata

struct Trade
{
    Price price;
    Quantity quantity;
    uint64_t timestamp;

    GEN_METADATA(price, quantity, timestamp);
};

namespace metadata {
template<>
struct NameOf<Trade>
{
    static const char *get()
    {
        return "Trade";
    }
};
}  // namespace metadata