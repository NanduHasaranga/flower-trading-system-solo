#include "Engine\OrderBook.hpp"
#include "Utils/TimeUtils.hpp"
#include <algorithm>

void OrderBook::processOrder(Order &order, std::vector<Record> &records)
{
    bool isProceed = false;

    if (order.side == Side::Sell)
    {
        while (order.quantity > 0 && !OrderBook::buyingSide.isEmpty())
        {
            const Order &topOrder = OrderBook::buyingSide.getBestOrder();
            if (topOrder.price < order.price)
            {
                break;
            }

            const int topQuantity = topOrder.quantity;
            const int proceedQuantity = std::min(order.quantity, topQuantity);
            const bool incomingFilled = order.quantity <= topQuantity;
            const bool topFilled = topQuantity <= order.quantity;
            const std::string timestamp = utils::getCurrentTimestamp();

            records.emplace_back(ExecutionReport{order.clientOrderId, order.orderId, order.instrument, order.side, topOrder.price, proceedQuantity,
                                                 incomingFilled ? OrderStatus::Fill : OrderStatus::PFill, timestamp});
            records.emplace_back(ExecutionReport{topOrder.clientOrderId, topOrder.orderId, topOrder.instrument, topOrder.side, topOrder.price, proceedQuantity,
                                                 topFilled ? OrderStatus::Fill : OrderStatus::PFill, timestamp});

            if (topFilled)
            {
                order.quantity -= topQuantity;
                OrderBook::buyingSide.removeTopOrder();
                isProceed = true;

                if (order.quantity == 0)
                {
                    return;
                }
            }
            else
            {
                OrderBook::buyingSide.updateTopOrderQuantity(topQuantity - proceedQuantity);
                return;
            }
        }
        if (!isProceed)
            records.emplace_back(ExecutionReport{order.clientOrderId, order.orderId, order.instrument, order.side, order.price, order.quantity, OrderStatus::New, utils::getCurrentTimestamp()});
        OrderBook::sellingSide.insertOrder(order);
        return;
    }
    else
    {
        while (order.quantity > 0 && !OrderBook::sellingSide.isEmpty())
        {
            const Order &topOrder = OrderBook::sellingSide.getBestOrder();
            if (topOrder.price > order.price)
            {
                break;
            }

            const int topQuantity = topOrder.quantity;
            const int proceedQuantity = std::min(order.quantity, topQuantity);
            const bool incomingFilled = order.quantity <= topQuantity;
            const bool topFilled = topQuantity <= order.quantity;
            const std::string timestamp = utils::getCurrentTimestamp();

            records.emplace_back(ExecutionReport{order.clientOrderId, order.orderId, order.instrument, order.side, topOrder.price, proceedQuantity,
                                                 incomingFilled ? OrderStatus::Fill : OrderStatus::PFill, timestamp});
            records.emplace_back(ExecutionReport{topOrder.clientOrderId, topOrder.orderId, topOrder.instrument, topOrder.side, topOrder.price, proceedQuantity,
                                                 topFilled ? OrderStatus::Fill : OrderStatus::PFill, timestamp});

            if (topFilled)
            {
                order.quantity -= topQuantity;
                OrderBook::sellingSide.removeTopOrder();
                isProceed = true;

                if (order.quantity == 0)
                {
                    return;
                }
            }
            else
            {
                OrderBook::sellingSide.updateTopOrderQuantity(topQuantity - proceedQuantity);
                return;
            }
        }
        if (!isProceed)
            records.emplace_back(ExecutionReport{order.clientOrderId, order.orderId, order.instrument, order.side, order.price, order.quantity, OrderStatus::New, utils::getCurrentTimestamp()});
        OrderBook::buyingSide.insertOrder(order);
        return;
    }
}