#include "Engine/OrderBook.hpp"
#include "Utils/TimeUtils.hpp"

namespace
{
    struct BreakForSell
    {
        bool operator()(double passivePrice, double incomingPrice) const
        {
            return passivePrice < incomingPrice;
        }
    };

    struct BreakForBuy
    {
        bool operator()(double passivePrice, double incomingPrice) const
        {
            return passivePrice > incomingPrice;
        }
    };

    template <typename PassiveSideT, typename RestingSideT, typename PriceBreakT>
    void matchOrderImpl(
        Order &order,
        PassiveSideT &passiveSide,
        RestingSideT &restingSide,
        PriceBreakT priceBreaks,
        std::vector<std::variant<ExecutionReport, OrderReject>> &outReports,
        const utils::Timestamp &timestamp)
    {
        bool hasExecution = false;

        while (!passiveSide.isEmpty())
        {
            const Order &topOrder = passiveSide.getBestOrder();
            if (priceBreaks(topOrder.price, order.price))
                break;

            const int proceedQuantity = std::min(order.quantity, topOrder.quantity);

            if (order.quantity >= topOrder.quantity)
            {
                const OrderStatus incomingStatus = (order.quantity == topOrder.quantity)
                                                       ? OrderStatus::Fill
                                                       : OrderStatus::PFill;

                outReports.emplace_back(std::in_place_type<ExecutionReport>,
                                        order.clientOrderId, order.orderId, order.instrument, order.side,
                                        topOrder.price, proceedQuantity, incomingStatus, timestamp);

                outReports.emplace_back(std::in_place_type<ExecutionReport>,
                                        topOrder.clientOrderId, topOrder.orderId, topOrder.instrument, topOrder.side,
                                        topOrder.price, proceedQuantity, OrderStatus::Fill, timestamp);

                order.quantity -= proceedQuantity;
                passiveSide.removeTopOrder();

                if (incomingStatus == OrderStatus::Fill)
                    return;

                hasExecution = true;
            }
            else
            {
                outReports.emplace_back(std::in_place_type<ExecutionReport>,
                                        order.clientOrderId, order.orderId, order.instrument, order.side,
                                        topOrder.price, proceedQuantity, OrderStatus::Fill, timestamp);

                outReports.emplace_back(std::in_place_type<ExecutionReport>,
                                        topOrder.clientOrderId, topOrder.orderId, topOrder.instrument, topOrder.side,
                                        topOrder.price, proceedQuantity, OrderStatus::PFill, timestamp);

                passiveSide.updateTopOrderQuantity(topOrder.quantity - proceedQuantity);
                return;
            }
        }

        if (!hasExecution)
            outReports.emplace_back(std::in_place_type<ExecutionReport>,
                                    order.clientOrderId, order.orderId, order.instrument, order.side,
                                    order.price, order.quantity, OrderStatus::New, timestamp);

        restingSide.insertOrder(order);
    }
} // namespace

void OrderBook::processOrder(
    Order &order,
    std::vector<std::variant<ExecutionReport, OrderReject>> &outReports)
{
    const auto timestamp = utils::getCurrentTimestamp();

    if (order.side == Side::Sell)
    {
        matchOrderImpl(order, buyingSide, sellingSide, BreakForSell{}, outReports, timestamp);
    }
    else
    {
        matchOrderImpl(order, sellingSide, buyingSide, BreakForBuy{}, outReports, timestamp);
    }
}