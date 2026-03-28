#include "IO/OrderProcessor.hpp"
#include <string>
#include "Core/Order.hpp"
#include "Core/OrderReject.hpp"
#include "IO/CsvReader.hpp"
#include "Utils/TimeUtils.hpp"
#include "Utils/OrderIdGenerator.hpp"

std::variant<Order, OrderReject> OrderProcessor::processRow(const CsvRow &row)
{
    auto validation = OrderValidator::validate(row);

    if (auto *rejectReason = std::get_if<std::string>(&validation))
    {
        return buildReject(row, *rejectReason);
    }

    const ValidationResult &result = std::get<ValidationResult>(validation);
    return buildValidatedOrder(row, result);
}

OrderReject OrderProcessor::buildReject(const CsvRow &row, const std::string &rejectReason)
{
    return OrderReject{
        Utils::OrderIdGenerator::generateId(),
        std::string(fieldAt(row, 0)),
        std::string(fieldAt(row, 1)),
        std::string(fieldAt(row, 2)),
        std::string(fieldAt(row, 3)),
        std::string(fieldAt(row, 4)),
        rejectReason,
        utils::getCurrentTimestamp()};
}

Order OrderProcessor::buildValidatedOrder(const CsvRow &row, const ValidationResult &result)
{
    return Order{
        std::string(fieldAt(row, 0)),
        Utils::OrderIdGenerator::generateId(),
        result.instrument,
        result.side,
        result.price,
        result.quantity};
}