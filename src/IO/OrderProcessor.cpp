#include "IO/OrderProcessor.hpp"
#include <string>
#include "Core/Order.hpp"
#include "Core/OrderReject.hpp"
#include "IO/CsvReader.hpp"
#include "Utils/OrderIdGenerator.hpp"
#include "Utils/FixedString.hpp"

std::variant<Order, OrderReject> OrderProcessor::processRow(const CsvRow &row, const utils::Timestamp &timestamp)
{
    auto validation = OrderValidator::validate(row);

    if (auto *rejectReason = std::get_if<std::string>(&validation))
    {
        return buildReject(row, *rejectReason, timestamp);
    }

    const ValidationResult &result = std::get<ValidationResult>(validation);
    return buildValidatedOrder(row, result);
}

OrderReject OrderProcessor::buildReject(const CsvRow &row, const std::string &rejectReason, const utils::Timestamp &timestamp)
{
    return OrderReject{
        Utils::OrderIdGenerator::generateId(),
        std::string(fieldAt(row, 0)),
        std::string(fieldAt(row, 1)),
        std::string(fieldAt(row, 2)),
        std::string(fieldAt(row, 3)),
        std::string(fieldAt(row, 4)),
        rejectReason,
        timestamp};
}

Order OrderProcessor::buildValidatedOrder(const CsvRow &row, const ValidationResult &result)
{
    return Order{
        FixedString<16>(fieldAt(row, 0)),
        FixedString<16>(Utils::OrderIdGenerator::generateId()),
        result.instrument,
        result.side,
        result.price,
        result.quantity};
}