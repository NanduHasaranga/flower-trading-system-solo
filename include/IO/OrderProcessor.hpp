#pragma once
#include <string>
#include <string_view>
#include <variant>
#include "IO/CsvReader.hpp"
#include "Core/Types.hpp"
#include "Core/ExecutionReport.hpp"
#include "Core/Order.hpp"

class OrderProcessor
{
private:
    static long nextOrderId;
    static std::string generateOrderID();
    static bool validate(std::string_view instrumentText,
                         std::string_view sideText,
                         std::string_view quantityText,
                         std::string_view priceText,
                         std::string &reason,
                         Instrument &instrument,
                         Side &side,
                         int &quantity,
                         double &price);

public:
    static std::variant<Order, ExecutionReport> processRow(const CsvRow &row);
};