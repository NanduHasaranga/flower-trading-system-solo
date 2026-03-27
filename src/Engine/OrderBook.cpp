#include "Engine/OrderBook.hpp"
#include "Utils/TimeUtils.hpp"

void OrderBook::processOrder(Order &order, std::vector<std::variant<ExecutionReport, OrderReject>> &outReports)
{
    bool hasExecution = false;
    const std::string timestamp = utils::getCurrentTimestamp();

    if (order.side == Side::Sell)
    {
        while (!OrderBook::buyingSide.isEmpty())
        {
            const Order &topOrder = OrderBook::buyingSide.getBestOrder();
            if (topOrder.price < order.price)
            {
                break;
            }

            if (order.quantity >= topOrder.quantity)
            {
                int proceedQuantity = topOrder.quantity;
                if (order.quantity == topOrder.quantity)
                {
                    outReports.emplace_back(std::in_place_type<ExecutionReport>, order.clientOrderId, order.orderId, order.instrument, order.side, topOrder.price, proceedQuantity, OrderStatus::Fill, timestamp);
                    outReports.emplace_back(std::in_place_type<ExecutionReport>, topOrder.clientOrderId, topOrder.orderId, topOrder.instrument, topOrder.side, topOrder.price, proceedQuantity, OrderStatus::Fill, timestamp);
                    OrderBook::buyingSide.removeTopOrder();
                    return;
                }
                else
                {
                    outReports.emplace_back(std::in_place_type<ExecutionReport>, order.clientOrderId, order.orderId, order.instrument, order.side, topOrder.price, proceedQuantity, OrderStatus::PFill, timestamp);
                    outReports.emplace_back(std::in_place_type<ExecutionReport>, topOrder.clientOrderId, topOrder.orderId, topOrder.instrument, topOrder.side, topOrder.price, proceedQuantity, OrderStatus::Fill, timestamp);
                    order.quantity = order.quantity - proceedQuantity;
                    OrderBook::buyingSide.removeTopOrder();
                    hasExecution = true;
                }
            }
            else
            {
                int proceedQuantity = order.quantity;
                outReports.emplace_back(std::in_place_type<ExecutionReport>, order.clientOrderId, order.orderId, order.instrument, order.side, topOrder.price, proceedQuantity, OrderStatus::Fill, timestamp);
                outReports.emplace_back(std::in_place_type<ExecutionReport>, topOrder.clientOrderId, topOrder.orderId, topOrder.instrument, topOrder.side, topOrder.price, proceedQuantity, OrderStatus::PFill, timestamp);
                OrderBook::buyingSide.updateTopOrderQuantity(topOrder.quantity - proceedQuantity);
                return;
            }
        }
        if (!hasExecution)
            outReports.emplace_back(std::in_place_type<ExecutionReport>, order.clientOrderId, order.orderId, order.instrument, order.side, order.price, order.quantity, OrderStatus::New, timestamp);
        OrderBook::sellingSide.insertOrder(order);
        return;
    }

    while (!OrderBook::sellingSide.isEmpty())
    { 
        const Order &topOrder = OrderBook::sellingSide.getBestOrder();
        if (topOrder.price > order.price)
        {
            break;
        }

        if (order.quantity >= topOrder.quantity)
        {
            int proceedQuantity = topOrder.quantity;
            if (order.quantity == topOrder.quantity)
            {
                outReports.emplace_back(std::in_place_type<ExecutionReport>, order.clientOrderId, order.orderId, order.instrument, order.side, topOrder.price, proceedQuantity, OrderStatus::Fill, timestamp);
                outReports.emplace_back(std::in_place_type<ExecutionReport>, topOrder.clientOrderId, topOrder.orderId, topOrder.instrument, topOrder.side, topOrder.price, proceedQuantity, OrderStatus::Fill, timestamp);
                OrderBook::sellingSide.removeTopOrder();
                return;
            }
            else
            {
                outReports.emplace_back(std::in_place_type<ExecutionReport>, order.clientOrderId, order.orderId, order.instrument, order.side, topOrder.price, proceedQuantity, OrderStatus::PFill, timestamp);
                outReports.emplace_back(std::in_place_type<ExecutionReport>, topOrder.clientOrderId, topOrder.orderId, topOrder.instrument, topOrder.side, topOrder.price, proceedQuantity, OrderStatus::Fill, timestamp);
                order.quantity = order.quantity - proceedQuantity;
                OrderBook::sellingSide.removeTopOrder();
                hasExecution = true;
            }
        }
        else
        {
            int proceedQuantity = order.quantity;
            outReports.emplace_back(std::in_place_type<ExecutionReport>, order.clientOrderId, order.orderId, order.instrument, order.side, topOrder.price, proceedQuantity, OrderStatus::Fill, timestamp);
            outReports.emplace_back(std::in_place_type<ExecutionReport>, topOrder.clientOrderId, topOrder.orderId, topOrder.instrument, topOrder.side, topOrder.price, proceedQuantity, OrderStatus::PFill, timestamp);
            OrderBook::sellingSide.updateTopOrderQuantity(topOrder.quantity - proceedQuantity);
            return;
        }
    }

    if (!hasExecution)
        outReports.emplace_back(std::in_place_type<ExecutionReport>, order.clientOrderId, order.orderId, order.instrument, order.side, order.price, order.quantity, OrderStatus::New, timestamp);
    OrderBook::buyingSide.insertOrder(order);
}