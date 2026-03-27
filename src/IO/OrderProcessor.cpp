#include "IO/OrderProcessor.hpp"
#include <string>
#include <string_view>
#include "Core/Order.hpp"
#include "Core/OrderReject.hpp"
#include "Core/RawOrder.hpp"
#include "Core/OrderValidator.hpp"
#include "IO/CsvReader.hpp"
#include "Utils/TimeUtils.hpp"
#include "Utils/OrderIdGenerator.hpp"

std::variant<Order, OrderReject> OrderProcessor::processRow(const CsvRow &row)
{
    const RawOrder rawOrder = RawOrder::fromCsvRow(row);

    // Validate
    auto validation = OrderValidator::validate(rawOrder);
    if (auto *rejectReason = std::get_if<std::string>(&validation))
    {
        return OrderReject{
            Utils::OrderIdGenerator::generateId(),
            rawOrder.clientOrderId,
            rawOrder.instrument,
            rawOrder.side,
            rawOrder.quantity,
            rawOrder.price,
            *rejectReason,
            utils::getCurrentTimestamp()};
    }

    // Create Order with validated values
    const ValidationResult &result = std::get<ValidationResult>(validation);
    return Order{
        rawOrder.clientOrderId,
        Utils::OrderIdGenerator::generateId(),
        result.instrument,
        result.side,
        result.price,
        result.quantity};
}