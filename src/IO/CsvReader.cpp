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

std::optional<std::vector<std::string>> CsvReader::nextRow()
{
    if (!file.is_open() || file.eof())
    {
        return std::nullopt;
    }

    std::string line;
    if (std::getline(file, line))
    {
        return parseLine(line);
    }

    return std::nullopt;
}

void CsvReader::close()
{
    if (file.is_open())
    {
        file.close();
    }
}

std::vector<std::string> CsvReader::parseLine(const std::string &line) const
{
    std::vector<std::string> result;
    result.reserve(8);
    std::string current;
    current.reserve(32);

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