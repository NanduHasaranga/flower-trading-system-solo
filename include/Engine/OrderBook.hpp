#pragma once

#include <vector>
#include <variant>
#include "OrderBookSide.hpp"
#include "./Core/ExecutionReport.hpp"
#include "./Core/OrderReject.hpp"

class OrderBook
{
private:
    OrderBookSide<BuyComparator> buyingSide;
    OrderBookSide<SellComparator> sellingSide;

public:
    OrderBook() = default;
    void processOrder(Order &order, std::vector<std::variant<ExecutionReport, OrderReject>> &outReports,
                      const utils::Timestamp &timestamp);
};