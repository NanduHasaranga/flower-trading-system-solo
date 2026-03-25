#ifndef REJECTIONREPORT_HPP
#define REJECTIONREPORT_HPP

#include <string>
#include <utility>

struct OrderReject
{
    std::string clientOrderId;
    std::string instrument;
    std::string side;
    std::string price;
    std::string quantity;
    std::string reason;
    std::string timestamp;

    OrderReject() = default;
        OrderReject(std::string clientOrderId,
                                std::string instrument,
                                std::string side,
                                std::string price,
                                std::string quantity,
                                std::string reason,
                                std::string timestamp)
                : clientOrderId(std::move(clientOrderId)),
                    instrument(std::move(instrument)),
                    side(std::move(side)),
                    price(std::move(price)),
                    quantity(std::move(quantity)),
                    reason(std::move(reason)),
                    timestamp(std::move(timestamp))
        {
        }
};

#endif