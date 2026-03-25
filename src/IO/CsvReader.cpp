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

std::vector<std::vector<std::string>> CsvReader::parseFile(std::ifstream &file) const
{
    std::vector<std::vector<std::string>> rows;
    std::string line;

    std::getline(file, line);

    while (std::getline(file, line))
    {
        rows.push_back(parseLine(line));
    }

    return rows;
}