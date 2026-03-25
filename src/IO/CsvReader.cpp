#include "IO/CsvReader.hpp"

std::vector<std::string> CsvReader::parseLine(const std::string &line) const
{
    std::vector<std::string> result;
    std::string current;

    for (char c : line)
    {
        if (c == ',')
        {
            result.push_back(current);
            current.clear();
        }
        else
        {
            current += c;
        }
    }
    result.push_back(current);
    return result;
}