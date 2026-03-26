#include "Engine\OrderBook.hpp"
#include "Utils/TimeUtils.hpp"
#include "Utils/StringUtils.hpp"

#include <iostream>

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

    bool isProceed = false;

    if (order.side == Side::Sell)
    {
        while (isMatchingOrder(order))
        {
            const Order &topOrder = OrderBook::buyingSide.getBestOrder();
            if (order.quantity >= topOrder.quantity)
            {
                int proceedQuantity = topOrder.quantity;
                if (order.quantity == topOrder.quantity)
                {
                    const std::string timestamp = utils::getCurrentTimestamp();
                    reports.push_back(ExecutionReport(order.clientOrderId, order.orderId, to_string(order.instrument), to_string(order.side), utils::toString(topOrder.price), utils::toString(proceedQuantity), "Fill", " ", timestamp));
                    reports.push_back(ExecutionReport(topOrder.clientOrderId, topOrder.orderId, to_string(topOrder.instrument), to_string(topOrder.side), utils::toString(topOrder.price), utils::toString(proceedQuantity), "Fill", " ", timestamp));
                    OrderBook::buyingSide.removeTopOrder();
                    return reports;
                }
                else
                {
                    const std::string timestamp = utils::getCurrentTimestamp();
                    reports.push_back(ExecutionReport(order.clientOrderId, order.orderId, to_string(order.instrument), to_string(order.side), utils::toString(topOrder.price), utils::toString(proceedQuantity), "PFill", " ", timestamp));
                    reports.push_back(ExecutionReport(topOrder.clientOrderId, topOrder.orderId, to_string(topOrder.instrument), to_string(topOrder.side), utils::toString(topOrder.price), utils::toString(proceedQuantity), "Fill", " ", timestamp));
                    order.quantity = order.quantity - proceedQuantity;
                    OrderBook::buyingSide.removeTopOrder();
                    isProceed = true;
                }
            }
            else
            {
                int proceedQuantity = order.quantity;
                const std::string timestamp = utils::getCurrentTimestamp();
                reports.push_back(ExecutionReport(order.clientOrderId, order.orderId, to_string(order.instrument), to_string(order.side), utils::toString(topOrder.price), utils::toString(proceedQuantity), "Fill", " ", timestamp));
                reports.push_back(ExecutionReport(topOrder.clientOrderId, topOrder.orderId, to_string(topOrder.instrument), to_string(topOrder.side), utils::toString(topOrder.price), utils::toString(proceedQuantity), "PFill", " ", timestamp));
                OrderBook::buyingSide.updateTopOrderQuantity(topOrder.quantity - proceedQuantity);
                return reports;
            }
        }
        if (!isProceed)
            reports.push_back(ExecutionReport(order.clientOrderId, order.orderId, to_string(order.instrument), to_string(order.side), utils::toString(order.price), utils::toString(order.quantity), "New", " ", utils::getCurrentTimestamp()));
        OrderBook::sellingSide.insertOrder(order);
        return reports;
    }
    else
    {
        while (isMatchingOrder(order))
        {
            const Order &topOrder = OrderBook::sellingSide.getBestOrder();
            if (order.quantity >= topOrder.quantity)
            {
                int proceedQuantity = topOrder.quantity;
                if (order.quantity == topOrder.quantity)
                {
                    const std::string timestamp = utils::getCurrentTimestamp();
                    reports.push_back(ExecutionReport(order.clientOrderId, order.orderId, to_string(order.instrument), to_string(order.side), utils::toString(topOrder.price), utils::toString(proceedQuantity), "Fill", " ", timestamp));
                    reports.push_back(ExecutionReport(topOrder.clientOrderId, topOrder.orderId, to_string(topOrder.instrument), to_string(topOrder.side), utils::toString(topOrder.price), utils::toString(proceedQuantity), "Fill", " ", timestamp));
                    OrderBook::sellingSide.removeTopOrder();
                    return reports;
                }
                else
                {
                    const std::string timestamp = utils::getCurrentTimestamp();
                    reports.push_back(ExecutionReport(order.clientOrderId, order.orderId, to_string(order.instrument), to_string(order.side), utils::toString(topOrder.price), utils::toString(proceedQuantity), "PFill", " ", timestamp));
                    reports.push_back(ExecutionReport(topOrder.clientOrderId, topOrder.orderId, to_string(topOrder.instrument), to_string(topOrder.side), utils::toString(topOrder.price), utils::toString(proceedQuantity), "Fill", " ", timestamp));
                    order.quantity = order.quantity - proceedQuantity;
                    OrderBook::sellingSide.removeTopOrder();
                    isProceed = true;
                }
            }
            else
            {
                int proceedQuantity = order.quantity;
                const std::string timestamp = utils::getCurrentTimestamp();
                reports.push_back(ExecutionReport(order.clientOrderId, order.orderId, to_string(order.instrument), to_string(order.side), utils::toString(topOrder.price), utils::toString(proceedQuantity), "Fill", " ", timestamp));
                reports.push_back(ExecutionReport(topOrder.clientOrderId, topOrder.orderId, to_string(topOrder.instrument), to_string(topOrder.side), utils::toString(topOrder.price), utils::toString(proceedQuantity), "PFill", " ", timestamp));
                OrderBook::sellingSide.updateTopOrderQuantity(topOrder.quantity - proceedQuantity);
                return reports;
            }
        }
        if (!isProceed)
            reports.push_back(ExecutionReport(order.clientOrderId, order.orderId, to_string(order.instrument), to_string(order.side), utils::toString(order.price), utils::toString(order.quantity), "New", " ", utils::getCurrentTimestamp()));
        OrderBook::buyingSide.insertOrder(order);
        return reports;
    }
}