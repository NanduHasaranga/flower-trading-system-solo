#ifndef ORDERBOOK_HPP
#define ORDERBOOK_HPP

#include <vector>
#include <variant>
#include <functional>
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

template <typename Comparator>
struct OrderBookSideAdapter : IOrderBookSide
{
    OrderBookSide<Comparator> &side;
    explicit OrderBookSideAdapter(OrderBookSide<Comparator> &s) : side(s) {}

    bool isEmpty() const override { return side.isEmpty(); }
    const Order &getBestOrder() const override { return side.getBestOrder(); }
    void removeTopOrder() override { side.removeTopOrder(); }
    void updateTopOrderQuantity(int qty) override { side.updateTopOrderQuantity(qty); }
    void insertOrder(const Order &order) override { side.insertOrder(order); }
};

class OrderBook
{
private:
    OrderBookSide<BuyComparator> buyingSide;
    OrderBookSide<SellComparator> sellingSide;

    void matchOrder(Order &order,
                    IOrderBookSide &passiveSide,
                    IOrderBookSide &restingSide,
                    std::function<bool(int passivePrice, int incomingPrice)> priceBreaks,
                    std::vector<std::variant<ExecutionReport, OrderReject>> &outReports,
                    const std::string &timestamp);

public:
    OrderBook() = default;
    void processOrder(Order &order, std::vector<std::variant<ExecutionReport, OrderReject>> &outReports);
};

#endif