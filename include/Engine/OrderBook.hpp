#ifndef ORDERBOOK_HPP
#define ORDERBOOK_HPP

#include <vector>
#include <variant>
#include "OrderBookSide.hpp"
#include "./Core/ExecutionReport.hpp"
#include "./Core/OrderReject.hpp"
#include "./Core/Types.hpp"

class OrderBook
{
private:
    Instrument instrument;
    OrderBookSide<BuyComparator> buyingSide;
    OrderBookSide<SellComparator> sellingSide;

public:
    OrderBook() = default;
    void processOrder(Order &order, std::vector<std::variant<ExecutionReport, OrderReject>> &outReports);
};

#endif