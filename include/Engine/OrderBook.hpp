#ifndef ORDERBOOK_HPP
#define ORDERBOOK_HPP

#include <vector>
#include "OrderBookSide.hpp"
#include "./Core/ExecutionReport.hpp"
#include "./Core/Types.hpp"
#include "./Utils/StringUtils.hpp"

class OrderBook {
private:
    //static long nextOrderId;
    Instrument instrument;
    OrderBookSide<BuyComparator> buyingSide;
    OrderBookSide<SellComparator> sellingSide;
    //std::string generateOrderID();
    bool isMatchingOrder(const Order &order);

public:
    OrderBook() = default;
    std::vector<ExecutionReport> processOrder(Order &order);
};

#endif