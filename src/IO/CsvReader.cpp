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

void CsvReader::parseFile(std::ifstream &file, RowCallback callback) const
{
    std::string line;
    std::getline(file, line);
    while (std::getline(file, line))
    {
        if (line.empty())
            continue;
        callback(parseLine(line));
    }
}