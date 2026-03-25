#ifndef CSVWRITER_HPP
#define CSVWRITER_HPP

#include <string>
#include <vector>
#include "OrderReject.hpp"
#include "ExecutionReport.hpp"

class CsvWriter
{
public:
    void writeRejects(const std::string& path, const std::vector<OrderReject>& rejects);
    void writeExecutions(const std::string& path, const std::vector<ExecutionReport>& reports);
};

#endif