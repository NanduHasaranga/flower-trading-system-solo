#ifndef ORDERBOOK_HPP
#define ORDERBOOK_HPP

#include <vector>
#include "OrderBookSide.hpp"
#include "./Core/ExecutionReport.hpp"
#include "./Core/Types.hpp"

class OrderBook
{
private:
    Instrument instrument;
    OrderBookSide<BuyComparator> buyingSide;
    OrderBookSide<SellComparator> sellingSide;
    bool isMatchingOrder(const Order &order);

public:
    OrderBook() = default;
    std::vector<ExecutionReport> processOrder(Order &order);
};

#endif