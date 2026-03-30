#pragma once

#include <vector>
#include <variant>
#include "OrderBookSide.hpp"
#include "./Core/ExecutionReport.hpp"
#include "./Core/OrderReject.hpp"
#include "./Core/Types.hpp"
struct IOrderBookSide
{
    virtual bool isEmpty() const = 0;
    virtual const Order &getBestOrder() const = 0;
    virtual void removeTopOrder() = 0;
    virtual void updateTopOrderQuantity(int qty) = 0;
    virtual void insertOrder(const Order &order) = 0;
    virtual ~IOrderBookSide() = default;
};

class OrderBook
{
private:
    OrderBookSide<BuyComparator> buyingSide;
    OrderBookSide<SellComparator> sellingSide;

public:
    OrderBook() = default;
    void processOrder(Order &order, std::vector<std::variant<ExecutionReport, OrderReject>> &outReports);
};