#pragma once

#include <string>
#include <vector>
#include <variant>
#include "Core/ExecutionReport.hpp"
#include "Core/OrderReject.hpp"

class CsvWriter
{
public:
    void writeExecutions(const std::string &path, const std::vector<std::variant<ExecutionReport, OrderReject>> &reports);
};