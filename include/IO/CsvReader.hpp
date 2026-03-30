#pragma once

#include <string>
#include <optional>
#include <fstream>
#include <array>
#include <string_view>

struct CsvRow
{
    std::array<std::string_view, 5> fields{};
    std::size_t fieldCount = 0;
};

inline std::string_view fieldAt(const CsvRow &row, std::size_t index)
{
    if (index < row.fieldCount && index < row.fields.size())
    {
        return row.fields[index];
    }
    return {};
}

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