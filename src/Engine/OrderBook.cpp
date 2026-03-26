#include "Engine\OrderBook.hpp"
#include "Utils/TimeUtils.hpp"
#include "Utils/StringUtils.hpp"

bool OrderBook::isMatchingOrder(const Order &order)
{
    if (order.side == Side::Sell)
    {
        if (OrderBook::buyingSide.isEmpty())
        {
            return false;
        }
        return OrderBook::buyingSide.getBestOrder().price >= order.price;
    }

    if (OrderBook::sellingSide.isEmpty())
    {
        return false;
    }
    return OrderBook::sellingSide.getBestOrder().price <= order.price;
}

void OrderBook::processOrder(Order &order, std::vector<ExecutionReport> &outReports)
{
    bool isProceed = false;
    const std::string orderInstrument = to_string(order.instrument);
    const std::string orderSide = to_string(order.side);

    if (order.side == Side::Sell)
    {
        while (isMatchingOrder(order))
        {
            const Order &topOrder = OrderBook::buyingSide.getBestOrder();
            const std::string topInstrument = to_string(topOrder.instrument);
            const std::string topSide = to_string(topOrder.side);
            const std::string topPrice = utils::toString(topOrder.price);
            if (order.quantity >= topOrder.quantity)
            {
                int proceedQuantity = topOrder.quantity;
                const std::string proceedQuantityText = utils::toString(proceedQuantity);
                if (order.quantity == topOrder.quantity)
                {
                    const std::string timestamp = utils::getCurrentTimestamp();
                    outReports.emplace_back(order.clientOrderId, order.orderId, orderInstrument, orderSide, topPrice, proceedQuantityText, "Fill", " ", timestamp);
                    outReports.emplace_back(topOrder.clientOrderId, topOrder.orderId, topInstrument, topSide, topPrice, proceedQuantityText, "Fill", " ", timestamp);
                    OrderBook::buyingSide.removeTopOrder();
                    return;
                }
                else
                {
                    const std::string timestamp = utils::getCurrentTimestamp();
                    outReports.emplace_back(order.clientOrderId, order.orderId, orderInstrument, orderSide, topPrice, proceedQuantityText, "PFill", " ", timestamp);
                    outReports.emplace_back(topOrder.clientOrderId, topOrder.orderId, topInstrument, topSide, topPrice, proceedQuantityText, "Fill", " ", timestamp);
                    order.quantity = order.quantity - proceedQuantity;
                    OrderBook::buyingSide.removeTopOrder();
                    isProceed = true;
                }
            }
            else
            {
                int proceedQuantity = order.quantity;
                const std::string proceedQuantityText = utils::toString(proceedQuantity);
                const std::string timestamp = utils::getCurrentTimestamp();
                outReports.emplace_back(order.clientOrderId, order.orderId, orderInstrument, orderSide, topPrice, proceedQuantityText, "Fill", " ", timestamp);
                outReports.emplace_back(topOrder.clientOrderId, topOrder.orderId, topInstrument, topSide, topPrice, proceedQuantityText, "PFill", " ", timestamp);
                OrderBook::buyingSide.updateTopOrderQuantity(topOrder.quantity - proceedQuantity);
                return;
            }
        }
        if (!isProceed)
            outReports.emplace_back(order.clientOrderId, order.orderId, orderInstrument, orderSide, utils::toString(order.price), utils::toString(order.quantity), "New", " ", utils::getCurrentTimestamp());
        OrderBook::sellingSide.insertOrder(order);
        return;
    }
    else
    {
        while (isMatchingOrder(order))
        {
            const Order &topOrder = OrderBook::sellingSide.getBestOrder();
            const std::string topInstrument = to_string(topOrder.instrument);
            const std::string topSide = to_string(topOrder.side);
            const std::string topPrice = utils::toString(topOrder.price);
            if (order.quantity >= topOrder.quantity)
            {
                int proceedQuantity = topOrder.quantity;
                const std::string proceedQuantityText = utils::toString(proceedQuantity);
                if (order.quantity == topOrder.quantity)
                {
                    const std::string timestamp = utils::getCurrentTimestamp();
                    outReports.emplace_back(order.clientOrderId, order.orderId, orderInstrument, orderSide, topPrice, proceedQuantityText, "Fill", " ", timestamp);
                    outReports.emplace_back(topOrder.clientOrderId, topOrder.orderId, topInstrument, topSide, topPrice, proceedQuantityText, "Fill", " ", timestamp);
                    OrderBook::sellingSide.removeTopOrder();
                    return;
                }
                else
                {
                    const std::string timestamp = utils::getCurrentTimestamp();
                    outReports.emplace_back(order.clientOrderId, order.orderId, orderInstrument, orderSide, topPrice, proceedQuantityText, "PFill", " ", timestamp);
                    outReports.emplace_back(topOrder.clientOrderId, topOrder.orderId, topInstrument, topSide, topPrice, proceedQuantityText, "Fill", " ", timestamp);
                    order.quantity = order.quantity - proceedQuantity;
                    OrderBook::sellingSide.removeTopOrder();
                    isProceed = true;
                }
            }
            else
            {
                int proceedQuantity = order.quantity;
                const std::string proceedQuantityText = utils::toString(proceedQuantity);
                const std::string timestamp = utils::getCurrentTimestamp();
                outReports.emplace_back(order.clientOrderId, order.orderId, orderInstrument, orderSide, topPrice, proceedQuantityText, "Fill", " ", timestamp);
                outReports.emplace_back(topOrder.clientOrderId, topOrder.orderId, topInstrument, topSide, topPrice, proceedQuantityText, "PFill", " ", timestamp);
                OrderBook::sellingSide.updateTopOrderQuantity(topOrder.quantity - proceedQuantity);
                return;
            }
        }
        if (!isProceed)
            outReports.emplace_back(order.clientOrderId, order.orderId, orderInstrument, orderSide, utils::toString(order.price), utils::toString(order.quantity), "New", " ", utils::getCurrentTimestamp());
        OrderBook::buyingSide.insertOrder(order);
        return;
    }
}