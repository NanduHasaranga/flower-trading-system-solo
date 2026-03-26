#include "IO/CsvReader.hpp"
#include <algorithm>

std::vector<std::string> CsvReader::parseLine(const std::string &line) const
{
    std::vector<std::string> result;
    result.reserve(static_cast<std::size_t>(std::count(line.begin(), line.end(), ',')) + 1);

    std::size_t start = 0;
    while (true)
    {
        const std::size_t comma = line.find(',', start);
        if (comma == std::string::npos)
        {
            result.emplace_back(line, start, std::string::npos);
            break;
        }

        result.emplace_back(line, start, comma - start);
        start = comma + 1;
    }

    return result;
}
