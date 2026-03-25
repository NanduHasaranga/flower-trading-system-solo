#ifndef CSVWRITER_HPP
#define CSVWRITER_HPP

#include <string>
#include <vector>
#include <variant>
#include "Core/OrderReject.hpp"
#include "Core/ExecutionReport.hpp"

using Record = std::variant<ExecutionReport, OrderReject>;

class CsvWriter
{
public:
    void writeRejects(const std::string& path, const std::vector<OrderReject>& rejects);
    void writeExecutions(const std::string& path, const std::vector<ExecutionReport>& reports);
    void writeChronological(const std::string& path, const std::vector<Record>& records);
};

#endif