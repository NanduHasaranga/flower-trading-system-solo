#pragma once

#include <string>
#include <optional>
#include <fstream>
#include "Core/RawOrder.hpp"

class CsvReader
{
public:
    CsvReader() = default;
    explicit CsvReader(const std::string &filepath);
    ~CsvReader() = default;
    bool open(const std::string &filepath);
    std::optional<CsvRow> nextRow();
    void close();

private:
    std::ifstream file;
    std::string currentLine;
    CsvRow parseLine(const std::string &line) const;
};