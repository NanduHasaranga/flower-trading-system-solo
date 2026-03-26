#include "IO/OrderProcessor.hpp"
#include "Utils/TimeUtils.hpp"
#include <charconv>

namespace
{
    struct RawFields
    {
        std::string clientOrderId;
        std::string instrument;
        std::string side;
        std::string quantity;
        std::string price;
    };

    bool tryParseInstrument(const std::string &text, Instrument &value)
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

    bool tryParseSide(const std::string &text, Side &value)
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

    bool parseIntStrict(const std::string &text, int &value)
    {
        if (text.empty())
            return false;

        const char *begin = text.data();
        const char *end = begin + text.size();
        auto [ptr, ec] = std::from_chars(begin, end, value);
        return ec == std::errc() && ptr == end;
    }

    bool parseDoubleStrict(const std::string &text, double &value)
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

bool OrderProcessor::validate(const std::string &instrumentText,
                              const std::string &sideText,
                              const std::string &quantityText,
                              const std::string &priceText,
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

std::variant<Order, ExecutionReport> OrderProcessor::processRow(const std::vector<std::string> &row)
{
    auto getField = [&row](std::size_t index) -> std::string
    {
        return index < row.size() ? row[index] : "";
    };

    RawFields raw{
        getField(0),
        getField(1),
        getField(2),
        getField(3),
        getField(4)};

    std::string orderId = generateOrderID();

    if (row.size() < 5)
    {
        return ExecutionReport{
            raw.clientOrderId,
            orderId,
            raw.instrument,
            raw.side,
            raw.price,
            raw.quantity,
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
            raw.clientOrderId,
            orderId,
            raw.instrument,
            raw.side,
            raw.price,
            raw.quantity,
            "Reject",
            reason,
            utils::getCurrentTimestamp()};
    }

    Order order{
        raw.clientOrderId,
        orderId,
        inst,
        side,
        price,
        quantity};
    return order;
}