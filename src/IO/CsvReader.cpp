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

std::optional<CsvRow> CsvReader::nextRow()
{
    if (!file.is_open() || file.eof())
    {
        return std::nullopt;
    }

    while (std::getline(file, currentLine))
    {
        if (currentLine.find_first_not_of(" \t\r\n") == std::string::npos)
        {
            continue;
        }
        return parseLine(currentLine);
    }

    return std::nullopt;
}

CsvRow CsvReader::parseLine(const std::string &line) const
{
    CsvRow row{};

    std::size_t start = 0;
    while (true)
    {
        const std::size_t comma = line.find(',', start);
        if (comma == std::string::npos)
        {
            if (row.fieldCount < row.fields.size())
            {
                row.fields[row.fieldCount] = std::string_view(line.data() + start, line.size() - start);
            }
            ++row.fieldCount;
            break;
        }
        if (row.fieldCount < row.fields.size())
        {
            row.fields[row.fieldCount] = std::string_view(line.data() + start, comma - start);
        }
        ++row.fieldCount;
        start = comma + 1;
    }

    return row;
}