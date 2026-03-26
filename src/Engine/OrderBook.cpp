#include "Engine\OrderBook.hpp"
#include "Utils/TimeUtils.hpp"

// long OrderBook::nextOrderId = 1;

// std::string OrderBook::generateOrderID() {
//     return "ord" + std::to_string(nextOrderId++);
// }

bool OrderBook::isMatchingOrder(const Order &order)
{

    if (order.side == Side::Sell && !OrderBook::buyingSide.isEmpty() && OrderBook::buyingSide.getBestOrder().price >= order.price && order.quantity > 0)
    {
        return true;
    }
    else if (order.side == Side::Buy && !OrderBook::sellingSide.isEmpty() && OrderBook::sellingSide.getBestOrder().price <= order.price && order.quantity > 0)
    {
        return true;
    }
    else
        return false;
}

std::vector<ExecutionReport> OrderBook::processOrder(Order &order)
{
    std::vector<ExecutionReport> reports;
    // order.orderId = generateOrderID();

    bool isProceed = false;

    if (order.side == Side::Sell)
    {
        while (isMatchingOrder(order))
        {
            Order topOrder = OrderBook::buyingSide.getBestOrder();
            if (order.quantity >= topOrder.quantity)
            {
                int proceedQuantity = topOrder.quantity;
                if (order.quantity == topOrder.quantity)
                {
                    reports.push_back(ExecutionReport(order.clientOrderId, order.orderId, order.instrument, order.side, topOrder.price, proceedQuantity, OrderStatus::Fill, utils::getCurrentTimestamp()));
                    reports.push_back(ExecutionReport(topOrder.clientOrderId, topOrder.orderId, topOrder.instrument, topOrder.side, topOrder.price, proceedQuantity, OrderStatus::Fill, utils::getCurrentTimestamp()));
                    OrderBook::buyingSide.removeTopOrder();
                    return reports;
                }
                else
                {
                    reports.push_back(ExecutionReport(order.clientOrderId, order.orderId, order.instrument, order.side, topOrder.price, proceedQuantity, OrderStatus::PFill, utils::getCurrentTimestamp()));
                    reports.push_back(ExecutionReport(topOrder.clientOrderId, topOrder.orderId, topOrder.instrument, topOrder.side, topOrder.price, proceedQuantity, OrderStatus::Fill, utils::getCurrentTimestamp()));
                    order.quantity = order.quantity - proceedQuantity;
                    OrderBook::buyingSide.removeTopOrder();
                    isProceed = true;
                }
            }
            else
            {
                int proceedQuantity = order.quantity;
                reports.push_back(ExecutionReport(order.clientOrderId, order.orderId, order.instrument, order.side, topOrder.price, proceedQuantity, OrderStatus::Fill, utils::getCurrentTimestamp()));
                reports.push_back(ExecutionReport(topOrder.clientOrderId, topOrder.orderId, topOrder.instrument, topOrder.side, topOrder.price, proceedQuantity, OrderStatus::PFill, utils::getCurrentTimestamp()));
                OrderBook::buyingSide.updateTopOrderQuantity(topOrder.quantity - proceedQuantity);
                return reports;
            }
        }
        if (!isProceed)
            reports.push_back(ExecutionReport(order.clientOrderId, order.orderId, order.instrument, order.side, order.price, order.quantity, OrderStatus::New, " "));
        OrderBook::sellingSide.insertOrder(order);
        return reports;
    }
    else
    {
        while (isMatchingOrder(order))
        {
            Order topOrder = OrderBook::sellingSide.getBestOrder();
            if (order.quantity >= topOrder.quantity)
            {
                int proceedQuantity = topOrder.quantity;
                if (order.quantity == topOrder.quantity)
                {
                    reports.push_back(ExecutionReport(order.clientOrderId, order.orderId, order.instrument, order.side, topOrder.price, proceedQuantity, OrderStatus::Fill, utils::getCurrentTimestamp()));
                    reports.push_back(ExecutionReport(topOrder.clientOrderId, topOrder.orderId, topOrder.instrument, topOrder.side, topOrder.price, proceedQuantity, OrderStatus::Fill, utils::getCurrentTimestamp()));
                    OrderBook::sellingSide.removeTopOrder();
                    return reports;
                }
                else
                {
                    reports.push_back(ExecutionReport(order.clientOrderId, order.orderId, order.instrument, order.side, topOrder.price, proceedQuantity, OrderStatus::PFill, utils::getCurrentTimestamp()));
                    reports.push_back(ExecutionReport(topOrder.clientOrderId, topOrder.orderId, topOrder.instrument, topOrder.side, topOrder.price, proceedQuantity, OrderStatus::Fill, utils::getCurrentTimestamp()));
                    order.quantity = order.quantity - proceedQuantity;
                    OrderBook::sellingSide.removeTopOrder();
                    isProceed = true;
                }
            }
            else
            {
                int proceedQuantity = order.quantity;
                reports.push_back(ExecutionReport(order.clientOrderId, order.orderId, order.instrument, order.side, topOrder.price, proceedQuantity, OrderStatus::Fill, utils::getCurrentTimestamp()));
                reports.push_back(ExecutionReport(topOrder.clientOrderId, topOrder.orderId, topOrder.instrument, topOrder.side, topOrder.price, proceedQuantity, OrderStatus::PFill, utils::getCurrentTimestamp()));
                OrderBook::sellingSide.updateTopOrderQuantity(topOrder.quantity - proceedQuantity);
                return reports;
            }
        }
        if (!isProceed)
            reports.push_back(ExecutionReport(order.clientOrderId, order.orderId, order.instrument, order.side, order.price, order.quantity, OrderStatus::New, utils::getCurrentTimestamp()));
        OrderBook::buyingSide.insertOrder(order);
        return reports;
    }
}