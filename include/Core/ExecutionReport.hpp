#pragma once

#include <string>
#include "Types.hpp"

struct ExecutionReport
{
    std::string clientOrderId;
    std::string orderId;
    Instrument instrument;
    Side side;
    double price;
    int quantity;
    OrderStatus status;
    std::string transactionTime;

    ExecutionReport() = default;

    ExecutionReport(
        std::string clientOrderId,
        std::string orderId,
        Instrument instrument,
        Side side,
        double price,
        int quantity,
        OrderStatus status,
        std::string transactionTime) : clientOrderId(clientOrderId), orderId(orderId), instrument(instrument), side(side), price(price), quantity(quantity), status(status), transactionTime(std::move(transactionTime)) {}
};