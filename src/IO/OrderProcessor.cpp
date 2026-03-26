#include "IO/OrderProcessor.hpp"
#include "Utils/TimeUtils.hpp"
#include <charconv>
#include <string_view>

namespace
{
    struct RawFields
    {
        std::string_view clientOrderId;
        std::string_view instrument;
        std::string_view side;
        std::string_view quantity;
        std::string_view price;
    };

    bool tryParseInstrument(std::string_view text, Instrument &value)
    {
        if (text == "Rose")
        {
            value = Instrument::Rose;
            return true;
        }
        if (text == "Lavender")
        {
            value = Instrument::Lavender;
            return true;
        }
        if (text == "Lotus")
        {
            value = Instrument::Lotus;
            return true;
        }
        if (text == "Tulip")
        {
            value = Instrument::Tulip;
            return true;
        }
        if (text == "Orchid")
        {
            value = Instrument::Orchid;
            return true;
        }
        return false;
    }

    bool tryParseSide(std::string_view text, Side &value)
    {
        if (text == "1")
        {
            value = Side::Buy;
            return true;
        }
        if (text == "2")
        {
            value = Side::Sell;
            return true;
        }
        return false;
    }

    bool parseIntStrict(std::string_view text, int &value)
    {
        if (text.empty())
            return false;

        const char *begin = text.data();
        const char *end = begin + text.size();
        auto [ptr, ec] = std::from_chars(begin, end, value);
        return ec == std::errc() && ptr == end;
    }

    bool parseDoubleStrict(std::string_view text, double &value)
    {
        if (text.empty())
            return false;

        const char *begin = text.data();
        const char *end = begin + text.size();
        auto [ptr, ec] = std::from_chars(begin, end, value);
        return ec == std::errc() && ptr == end;
    }
}

long OrderProcessor::nextOrderId = 1;

std::string OrderProcessor::generateOrderID()
{
    return "ord" + std::to_string(nextOrderId++);
}

bool OrderProcessor::validate(std::string_view instrumentText,
                              std::string_view sideText,
                              std::string_view quantityText,
                              std::string_view priceText,
                              std::string &reason,
                              Instrument &instrument,
                              Side &side,
                              int &quantity,
                              double &price)
{
    if (!tryParseInstrument(instrumentText, instrument))
    {
        reason = "Invalid instrument";
        return false;
    }

    if (!tryParseSide(sideText, side))
    {
        reason = "Invalid side";
        return false;
    }

    if (!parseIntStrict(quantityText, quantity) || quantity < 10 || quantity > 1000 || quantity % 10 != 0)
    {
        reason = "Invalid size";
        return false;
    }

    if (!parseDoubleStrict(priceText, price) || price <= 0)
    {
        reason = "Invalid price";
        return false;
    }

    return true;
}

std::variant<Order, ExecutionReport> OrderProcessor::processRow(const CsvRow &row)
{
    RawFields raw{
        row.fields[0],
        row.fields[1],
        row.fields[2],
        row.fields[3],
        row.fields[4]};

    std::string orderId = generateOrderID();

    if (row.fieldCount < row.fields.size())
    {
        return ExecutionReport{
            std::string(raw.clientOrderId),
            orderId,
            std::string(raw.instrument),
            std::string(raw.side),
            std::string(raw.price),
            std::string(raw.quantity),
            "Reject",
            "Missing fields",
            utils::getCurrentTimestamp()};
    }

    Instrument inst = Instrument::Rose;
    Side side = Side::Buy;
    int quantity = 0;
    double price = 0.0;
    std::string reason;

    if (!validate(raw.instrument, raw.side, raw.quantity, raw.price, reason, inst, side, quantity, price))
    {
        return ExecutionReport{
            std::string(raw.clientOrderId),
            orderId,
            std::string(raw.instrument),
            std::string(raw.side),
            std::string(raw.price),
            std::string(raw.quantity),
            "Reject",
            reason,
            utils::getCurrentTimestamp()};
    }

    Order order{
        std::string(raw.clientOrderId),
        orderId,
        inst,
        side,
        price,
        quantity};
    return order;
}