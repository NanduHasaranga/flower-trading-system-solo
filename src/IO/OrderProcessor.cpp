#include "IO/OrderProcessor.hpp"
#include <string>
#include "Core/Order.hpp"
#include "Core/RawOrder.hpp"
#include "Core/OrderReject.hpp"
#include "Utils/TimeUtils.hpp"

long OrderProcessor::nextOrderId = 1;

std::string OrderProcessor::generateOrderID()
{
    return "ord" + std::to_string(nextOrderId++);
}

static bool tryInstrument(const std::string &str, Instrument &out)
{
    if (str == "Rose")
    {
        out = Instrument::Rose;
        return true;
    }
    if (str == "Lavender")
    {
        out = Instrument::Lavender;
        return true;
    }
    if (str == "Lotus")
    {
        out = Instrument::Lotus;
        return true;
    }
    if (str == "Tulip")
    {
        out = Instrument::Tulip;
        return true;
    }
    if (str == "Orchid")
    {
        out = Instrument::Orchid;
        return true;
    }
    return false;
}

static bool trySide(const std::string &str, Side &out)
{
    if (str == "1")
    {
        out = Side::Buy;
        return true;
    }
    if (str == "2")
    {
        out = Side::Sell;
        return true;
    }
    return false;
}

std::variant<Order, OrderReject> OrderProcessor::processRow(const std::vector<std::string> &row)
{
    // Ingest
    if (row.size() < 5)
    {
        return OrderReject{
            generateOrderID(),
            row.size() > 0 ? row[0] : "",
            row.size() > 1 ? row[1] : "",
            row.size() > 2 ? row[2] : "",
            row.size() > 4 ? row[4] : "",
            row.size() > 3 ? row[3] : "",
            "Missing required field(s)",
            utils::getCurrentTimestamp()};
    }

    RawOrder raw{row[0], row[1], row[2], row[4], row[3]};

    // Validation
    std::string reason;
    Instrument inst;
    Side side;
    double price = 0.0;
    int qty = 0;

    if (!tryInstrument(raw.instrument, inst))
    {
        reason = "Invalid Instrument";
    }
    else if (!trySide(raw.side, side))
    {
        reason = "Invalid Side";
    }
    else if (raw.price.empty())
    {
        reason = "Missing Price";
    }
    else
    {
        try
        {
            price = std::stod(raw.price);
            if (price <= 0.0)
                reason = "Invalid Price";
        }
        catch (...)
        {
            reason = "Invalid Price Format";
        }
    }

    if (reason.empty())
    {
        if (raw.quantity.empty())
        {
            reason = "Missing Quantity";
        }
        else
        {
            try
            {
                qty = std::stoi(raw.quantity);
                if (qty < 10 || qty > 1000 || qty % 10 != 0)
                    reason = "Invalid Quantity";
            }
            catch (...)
            {
                reason = "Invalid Quantity Format";
            }
        }
    }

    // Decision
    if (!reason.empty())
    {
        return OrderReject{
            generateOrderID(),
            raw.clientOrderId,
            raw.instrument,
            raw.side,
            raw.price,
            raw.quantity,
            reason,
            utils::getCurrentTimestamp()};
    }

    // Create Order with validated values
    return Order{
        raw.clientOrderId,
        generateOrderID(),
        inst,
        side,
        price,
        qty};
}