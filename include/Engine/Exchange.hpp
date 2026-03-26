#ifndef EXCHANGE_HPP
#define EXCHANGE_HPP

#include "OrderBook.hpp"

class Exchange
{
private:
    std::map<Instrument, OrderBook> orderBooks;

public:
    Exchange() = default;
    OrderBook &getOrderBook(Order &order)
    {
        return Exchange::orderBooks[order.instrument];
    }
};

#endif