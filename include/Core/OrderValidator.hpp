#pragma once

#include <string>
#include <string_view>
#include <variant>
#include "Core/Types.hpp"

struct CsvRow;

struct ValidationResult
{
    Instrument instrument;
    Side side;
    double price;
    int quantity;
};

class OrderValidator
{
public:
    static std::variant<ValidationResult, std::string> validate(const CsvRow &row);

private:
    static bool tryInstrument(std::string_view value, Instrument &out);
    static bool trySide(std::string_view value, Side &out);
    static std::string validatePrice(std::string_view priceStr, double &out);
    static std::string validateQuantity(std::string_view quantityStr, int &out);
};
