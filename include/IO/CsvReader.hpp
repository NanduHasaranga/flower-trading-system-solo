#ifndef CSVREADER_HPP
#define CSVREADER_HPP

#include <string>
#include <vector>

class CsvReader
{
public:
    std::vector<std::string> parseLine(const std::string &line) const;
};

#endif