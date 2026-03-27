#include "IO/OrderProcessor.hpp"
#include <string>
#include <string_view>
#include <charconv>
#include "Core/Order.hpp"
#include "Core/OrderReject.hpp"
#include "IO/CsvReader.hpp"
#include "Utils/TimeUtils.hpp"

long OrderProcessor::nextOrderId = 1;

std::string OrderProcessor::generateOrderID()
{
    return "ord" + std::to_string(nextOrderId++);
}

static bool tryInstrument(std::string_view value, Instrument &out)
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

static bool trySide(std::string_view value, Side &out)
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

static std::string_view fieldAt(const CsvRow &row, std::size_t index)
{
    if (index < row.fieldCount && index < row.fields.size())
    {
        return row.fields[index];
    }
    return {};
}

std::variant<Order, OrderReject> OrderProcessor::processRow(const CsvRow &row)
{
    // Ingest
    if (row.fieldCount < 5)
    {
        const std::string_view clientOrderId = fieldAt(row, 0);
        const std::string_view instrument = fieldAt(row, 1);
        const std::string_view side = fieldAt(row, 2);
        const std::string_view quantity = fieldAt(row, 3);
        const std::string_view price = fieldAt(row, 4);

        return OrderReject{
            generateOrderID(),
            std::string(clientOrderId),
            std::string(instrument),
            std::string(side),
            std::string(price),
            std::string(quantity),
            "Missing fields",
            utils::getCurrentTimestamp()};
    }

    const std::string_view clientOrderId = row.fields[0];
    const std::string_view instrumentField = row.fields[1];
    const std::string_view sideField = row.fields[2];
    const std::string_view quantityField = row.fields[3];
    const std::string_view priceField = row.fields[4];

    // Validation
    std::string reason;
    Instrument inst;
    Side side;
    double price = 0.0;
    int qty = 0;

    if (!tryInstrument(instrumentField, inst))
    {
        reason = "Invalid instrument";
    }
    else if (!trySide(sideField, side))
    {
        reason = "Invalid side";
    }
    else if (auto [ptr, ec] = std::from_chars(priceField.data(),
                                              priceField.data() + priceField.size(),
                                              price);
             ec != std::errc() || ptr != priceField.data() + priceField.size() || price <= 0.0)
    {
        reason = "Invalid price";
    }
    else if (auto [ptr, ec] = std::from_chars(quantityField.data(),
                                              quantityField.data() + quantityField.size(),
                                              qty);
             ec != std::errc() || ptr != quantityField.data() + quantityField.size() ||
             qty < 10 || qty > 1000 || qty % 10 != 0)
    {
        reason = "Invalid size";
    }

    // Decision
    if (!reason.empty())
    {
        return OrderReject{
            generateOrderID(),
            std::string(clientOrderId),
            std::string(instrumentField),
            std::string(sideField),
            std::string(priceField),
            std::string(quantityField),
            reason,
            utils::getCurrentTimestamp()};
    }

    // Create Order with validated values
    return Order{
        std::string(clientOrderId),
        generateOrderID(),
        inst,
        side,
        price,
        qty};
}