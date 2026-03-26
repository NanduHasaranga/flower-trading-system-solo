#ifndef CSVREADER_HPP
#define CSVREADER_HPP

#include <string>
#include <vector>
#include <fstream>
#include <functional>

class CsvReader
{
public:
    using RowCallback = std::function<void(const std::vector<std::string> &)>;

    std::vector<std::string> parseLine(const std::string &line) const;
    void parseFile(std::ifstream &file, RowCallback callback) const;
};

#endif