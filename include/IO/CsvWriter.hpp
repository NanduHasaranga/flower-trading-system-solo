#ifndef CSVWRITER_HPP
#define CSVWRITER_HPP

#include <string>
#include <string_view>
#include <vector>
#include <variant>
#include <cstdio>
#include "Core/OrderReject.hpp"
#include "Core/ExecutionReport.hpp"

using Record = std::variant<ExecutionReport, OrderReject>;

class CsvWriter
{
public:
    void writeChronological(const std::string& path, const std::vector<Record>& records);

private:
    static void writeHeader(FILE* file);
    static void writeExecutionRow(FILE* file, const ExecutionReport& r, std::string& rowBuffer);
    static void writeRejectRow(FILE* file, const OrderReject& r, std::string& rowBuffer);
};

#endif