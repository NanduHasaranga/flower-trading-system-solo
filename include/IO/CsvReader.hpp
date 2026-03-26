#ifndef CSVREADER_HPP
#define CSVREADER_HPP

#include <string>
#include <vector>
#include <fstream>
#include <optional>

class CsvReader
{
public:
    bool open(const std::string &filepath);
    std::optional<std::vector<std::string>> nextRow();
    void close();

private:
    std::ifstream file;
    std::vector<std::string> parseLine(const std::string &line) const;
};

#endif