#ifndef EXCHANGE_HPP
#define EXCHANGE_HPP

#include <array>
#include "OrderBook.hpp"

class Exchange {
private:
    std::array<OrderBook, InstrumentStrings.size()> orderBooks{};

public:
    Exchange() = default;
    OrderBook& getOrderBook(Order &order){
        return Exchange::orderBooks[static_cast<std::size_t>(order.instrument)];
    }
};

#endif