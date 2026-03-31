#pragma once

#include <string>
#include <utility>
#include "Utils/TimeUtils.hpp"

struct OrderReject
{
    std::string orderId;
    std::string clientOrderId;
    std::string instrument;
    std::string side;
    std::string quantity;
    std::string price;
    std::string reason;
    utils::Timestamp timestamp;

    OrderReject() = default;
    OrderReject(std::string orderId,
                std::string clientOrderId,
                std::string instrument,
                std::string side,
                std::string quantity,
                std::string price,
                std::string reason,
                utils::Timestamp timestamp)
        : orderId(std::move(orderId)),
          clientOrderId(std::move(clientOrderId)),
          instrument(std::move(instrument)),
          side(std::move(side)),
          quantity(std::move(quantity)),
          price(std::move(price)),
          reason(std::move(reason)),
          timestamp(timestamp)
    {
    }
};
