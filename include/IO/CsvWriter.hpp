#ifndef CSVWRITER_HPP
#define CSVWRITER_HPP

#include <string>
#include <vector>
// #include <variant>
#include "Core/ExecutionReport.hpp"

// using Record = std::variant<ExecutionReport, OrderReject>;

class CsvWriter
{
public:
    void writeExecutions(const std::string& path, const std::vector<ExecutionReport>& rejects);
    // void writeExecutions(const std::string& path, const std::vector<ExecutionReport>& reports);
    // void writeChronological(const std::string& path, const std::vector<Record>& records);
};

#endif