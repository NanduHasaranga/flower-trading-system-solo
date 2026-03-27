#ifndef CSVWRITER_HPP
#define CSVWRITER_HPP

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

#endif