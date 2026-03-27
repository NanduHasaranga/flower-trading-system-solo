#pragma once

#include <string>
#include <string_view>
#include <array>

struct CsvRow
{
    std::array<std::string_view, 5> fields{};
    std::size_t fieldCount = 0;
};

inline std::string_view fieldAt(const CsvRow &row, std::size_t index)
{
    if (index < row.fieldCount && index < row.fields.size())
    {
        return row.fields[index];
    }
    return {};
}

struct RawOrder
{
    std::string clientOrderId;
    std::string instrument;
    std::string side;
    std::string price;
    std::string quantity;
};
