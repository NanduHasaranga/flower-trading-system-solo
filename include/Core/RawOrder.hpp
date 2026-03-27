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
    bool hasRequiredFieldCount = true;
    std::string clientOrderId;
    std::string instrument;
    std::string side;
    std::string quantity;
    std::string price;

    static RawOrder fromCsvRow(const CsvRow &row)
    {
        RawOrder rawOrder{
            row.fieldCount >= 5,
            std::string(fieldAt(row, 0)),
            std::string(fieldAt(row, 1)),
            std::string(fieldAt(row, 2)),
            std::string(fieldAt(row, 3)),
            std::string(fieldAt(row, 4))};
        return rawOrder;
    }
};
