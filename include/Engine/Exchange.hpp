#ifndef EXCHANGE_HPP
#define EXCHANGE_HPP

#include <array>
#include <cstddef>
#include "OrderBook.hpp"

class Exchange
{
private:
    static constexpr std::size_t kInstrumentCount = InstrumentStrings.size();
    std::array<OrderBook, kInstrumentCount> orderBooks{};

public:
    Exchange() = default;

    OrderBook &getOrderBook(const Order &order)
    {
        return orderBooks[static_cast<std::size_t>(order.instrument)];
    }

    const OrderBook &getOrderBook(const Order &order) const
    {
        return orderBooks[static_cast<std::size_t>(order.instrument)];
    }
};

#endif