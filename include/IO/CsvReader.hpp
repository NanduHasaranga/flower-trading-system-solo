#ifndef CSVREADER_HPP
#define CSVREADER_HPP

#include <string>
#include <vector>
#include <fstream>

class CsvReader
{
public:
    std::vector<std::string> parseLine(const std::string &line) const;
    std::vector<std::vector<std::string>> parseFile(std::ifstream &file) const;
};

#endif