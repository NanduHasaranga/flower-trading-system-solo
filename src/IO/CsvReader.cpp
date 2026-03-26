#include "IO/CsvReader.hpp"
#include <iostream>
#include <stdexcept>

CsvReader::CsvReader(const std::string &filepath)
{
    if (!open(filepath))
    {
        throw std::runtime_error("Failed to open " + filepath);
    }
}

bool CsvReader::open(const std::string &filepath)
{
    file.open(filepath);
    if (!file.is_open())
    {
        std::cerr << "Failed to open " << filepath << "\n";
        return false;
    }

    std::string line;
    std::getline(file, line);
    return true;
}

void CsvReader::close()
{
    if (file.is_open())
    {
        file.close();
    }
}

std::optional<std::vector<std::string_view>> CsvReader::nextRow()
{
    if (!file.is_open() || file.eof())
    {
        return std::nullopt;
    }

    if (std::getline(file, currentLine))
    {
        return parseLine(currentLine);
    }

    return std::nullopt;
}

std::vector<std::string_view> CsvReader::parseLine(const std::string &line) const
{
    std::vector<std::string_view> result;
    result.reserve(8);

    std::size_t start = 0;
    while (true)
    {
        const std::size_t comma = line.find(',', start);
        if (comma == std::string::npos)
        {
            result.emplace_back(line.data() + start, line.size() - start);
            break;
        }
        result.emplace_back(line.data() + start, comma - start);
        start = comma + 1;
    }

    return result;
}