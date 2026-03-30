#pragma once
#include <string>
#include <variant>
#include <vector>
#include "Core/Order.hpp"
#include "Core/OrderReject.hpp"
#include "Core/OrderValidator.hpp"

struct CsvRow;

class OrderProcessor
{
public:
    static std::variant<Order, OrderReject> processRow(const CsvRow &row);

private:
    static OrderReject buildReject(const CsvRow &row, const std::string &rejectReason);
    static Order buildValidatedOrder(const CsvRow &row, const ValidationResult &result);
};