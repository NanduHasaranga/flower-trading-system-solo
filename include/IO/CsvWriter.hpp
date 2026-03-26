#ifndef CSVWRITER_HPP
#define CSVWRITER_HPP

#include <string>
#include <vector>
#include "Core/ExecutionReport.hpp"

class CsvWriter
{
public:
    void writeExecutions(const std::string &path, const std::vector<ExecutionReport> &rejects);
};

#endif