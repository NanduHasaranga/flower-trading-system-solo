#include "IO/CsvReader.hpp"
#include <algorithm>

std::vector<std::string> CsvReader::parseLine(const std::string &line) const
{
    std::vector<std::string> result;
    result.reserve(static_cast<std::size_t>(std::count(line.begin(), line.end(), ',')) + 1);

    std::string current;
    current.reserve(line.size());

    for (char c : line)
    {
        if (c == ',')
        {
            result.emplace_back(std::move(current));
            current.clear();
        }
        else
        {
            current += c;
        }
    }
    result.emplace_back(std::move(current));
    return result;
}
