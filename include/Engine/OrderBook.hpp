#ifndef ORDERBOOK_HPP
#define ORDERBOOK_HPP

#include "OrderBookSide.hpp"
#include "./Core/Types.hpp"
#include "IO/CsvWriter.hpp"

class OrderBook {
private:
    //static long nextOrderId;
    Instrument instrument;
    OrderBookSide<BuyComparator> buyingSide;
    OrderBookSide<SellComparator> sellingSide;
    //std::string generateOrderID();

public:
    OrderBook() = default;
    void processOrder(Order &order, std::vector<Record> &records);
};

#endif