#include "Core/OrderValidator.hpp"
#include "Core/RawOrder.hpp"
#include <charconv>

bool OrderValidator::tryInstrument(std::string_view value, Instrument &out)
{
    if (value == "Rose")
    {
        out = Instrument::Rose;
        return true;
    }
    if (value == "Lavender")
    {
        out = Instrument::Lavender;
        return true;
    }
    if (value == "Lotus")
    {
        out = Instrument::Lotus;
        return true;
    }
    if (value == "Tulip")
    {
        out = Instrument::Tulip;
        return true;
    }
    if (value == "Orchid")
    {
        out = Instrument::Orchid;
        return true;
    }
    return false;
}

bool OrderValidator::trySide(std::string_view value, Side &out)
{
    if (value.size() != 1)
    {
        return false;
    }
    if (value[0] == '1')
    {
        out = Side::Buy;
        return true;
    }
    if (value[0] == '2')
    {
        out = Side::Sell;
        return true;
    }
    return false;
}

std::string OrderValidator::validatePrice(std::string_view priceStr, double &out)
{
    double price = 0.0;
    auto [ptr, ec] = std::from_chars(priceStr.data(),
                                     priceStr.data() + priceStr.size(),
                                     price);
    if (ec != std::errc() || ptr != priceStr.data() + priceStr.size() || price <= 0.0)
    {
        return "Invalid price";
    }
    out = price;
    return {};
}

std::string OrderValidator::validateQuantity(std::string_view quantityStr, int &out)
{
    int qty = 0;
    auto [ptr, ec] = std::from_chars(quantityStr.data(),
                                     quantityStr.data() + quantityStr.size(),
                                     qty);
    if (ec != std::errc() || ptr != quantityStr.data() + quantityStr.size() ||
        qty < 10 || qty > 1000 || qty % 10 != 0)
    {
        return "Invalid size";
    }
    out = qty;
    return {};
}

std::variant<ValidationResult, std::string> OrderValidator::validate(const RawOrder &rawOrder)
{
    ValidationResult result;

    if (!rawOrder.hasRequiredFieldCount)
    {
        return std::string("Missing fields");
    }

    if (!tryInstrument(rawOrder.instrument, result.instrument))
    {
        return std::string("Invalid instrument");
    }

    if (!trySide(rawOrder.side, result.side))
    {
        return std::string("Invalid side");
    }

    if (auto err = validatePrice(rawOrder.price, result.price); !err.empty())
    {
        return err;
    }

    if (auto err = validateQuantity(rawOrder.quantity, result.quantity); !err.empty())
    {
        return err;
    }

    return result;
}
