#ifndef EXECUTIONREPORT_HPP
#define EXECUTIONREPORT_HPP

#include <string>
#include <utility>

struct ExecutionReport
{
    std::string clientOrderId;
    std::string orderId;
    std::string instrument;
    std::string side;
    std::string price;
    std::string quantity;
    std::string status;
    std::string reason;
    std::string transactionTime;

    ExecutionReport() = default;

    ExecutionReport(
        std::string clientOrderId,
        std::string orderId,
        std::string instrument,
        std::string side,
        std::string price,
        std::string quantity,
        std::string status,
        std::string reason,
        std::string transactionTime) : clientOrderId(std::move(clientOrderId)), orderId(std::move(orderId)), instrument(std::move(instrument)), side(std::move(side)), price(std::move(price)), quantity(std::move(quantity)), status(std::move(status)), reason(std::move(reason)), transactionTime(std::move(transactionTime)) {}
};
#endif