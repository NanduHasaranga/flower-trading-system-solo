#pragma once

#include <string_view>
#include "Types.hpp"
#include "Utils/FixedString.hpp"
#include "Utils/TimeUtils.hpp"

struct ExecutionReport
{
    FixedString<16> clientOrderId;
    FixedString<16> orderId;
    Instrument instrument;
    Side side;
    double price;
    int quantity;
    OrderStatus status;
    utils::Timestamp transactionTime;

    ExecutionReport() = default;

    ExecutionReport(
        std::string_view clientOrderId,
        std::string_view orderId,
        Instrument instrument,
        Side side,
        double price,
        int quantity,
        OrderStatus status,
        utils::Timestamp transactionTime)
        : clientOrderId(clientOrderId),
          orderId(orderId),
          instrument(instrument),
          side(side),
          price(price),
          quantity(quantity),
          status(status),
          transactionTime(transactionTime) {}
};