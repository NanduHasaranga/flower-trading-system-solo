#ifndef CSVREADER_HPP
#define CSVREADER_HPP

#include <string>
#include <string_view>
#include <array>
#include <fstream>
#include <optional>

struct CsvRow
{
    std::array<std::string_view, 5> fields{};
    std::size_t fieldCount = 0;
};

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

#endif