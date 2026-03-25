#ifndef ORDERPROCESSOR_HPP
#define ORDERPROCESSOR_HPP

#include <variant>
#include <vector>
#include <string>
#include "Core/RawOrder.hpp"
#include "Core/Order.hpp"
#include "CsvRecord.hpp"

class OrderProcessor
{
public:
    std::variant<Order, CsvRecord> processRow(const std::vector<std::string>& row);

private:
    static long nextOrderId;
    std::string generateOrderID();
};

#endif