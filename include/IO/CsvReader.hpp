#ifndef CSVREADER_HPP
#define CSVREADER_HPP

#include <string>
#include <string_view>
#include <vector>
#include <fstream>
#include <optional>

class CsvReader
{
public:
    CsvReader() = default;
    explicit CsvReader(const std::string &filepath);
    ~CsvReader() = default;
    bool open(const std::string &filepath);
    std::optional<std::vector<std::string_view>> nextRow();
    void close();

private:
    std::ifstream file;
    std::string currentLine;
    std::vector<std::string_view> parseLine(const std::string &line) const;
};

#endif