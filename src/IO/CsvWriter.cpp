#include "IO/CsvWriter.hpp"
#include "Core/Types.hpp"
#include <charconv>
#include <fstream>
#include <vector>

namespace
{
    void appendInt(std::string &out, int value)
    {
        char buffer[16];
        auto [ptr, ec] = std::to_chars(buffer, buffer + sizeof(buffer), value);
        if (ec == std::errc())
        {
            out.append(buffer, static_cast<std::size_t>(ptr - buffer));
            return;
        }
        out += '0';
    }

    void appendDouble(std::string &out, double value)
    {
        char buffer[32];
        auto [ptr, ec] = std::to_chars(buffer, buffer + sizeof(buffer), value);
        if (ec == std::errc())
        {
            out.append(buffer, static_cast<std::size_t>(ptr - buffer));
            return;
        }
        out += '0';
    }

    const char *statusToString(OrderStatus status)
    {
        switch (status)
        {
        case OrderStatus::New:
            return "New";
        case OrderStatus::Fill:
            return "Fill";
        case OrderStatus::PFill:
            return "PFill";
        case OrderStatus::Reject:
            return "Reject";
        default:
            return "Reject";
        }
    }
}

void CsvWriter::writeExecutions(const std::string &path, const std::vector<std::variant<ExecutionReport, OrderReject>> &reports)
{
    std::vector<char> ioBuffer(1 << 20);
    std::ofstream file(path);
    file.rdbuf()->pubsetbuf(ioBuffer.data(), static_cast<std::streamsize>(ioBuffer.size()));

    std::string output;
    output.reserve(80 + reports.size() * 128);
    output += "Order ID,Cl. Ord.ID,Instrument,Side,Exec Status,Quantity,Price,reason,Transaction Time\n";

    for (const auto &report : reports)
    {
        if (const auto *execution = std::get_if<ExecutionReport>(&report))
        {
            output += execution->orderId;
            output += ',';
            output += execution->clientOrderId;
            output += ',';
            output += to_string(execution->instrument);
            output += ',';
            output += to_string(execution->side);
            output += ',';
            output += statusToString(execution->status);
            output += ',';
            appendInt(output, execution->quantity);
            output += ',';
            appendDouble(output, execution->price);
            output += ',';
            output += ',';
            output += execution->transactionTime;
            output += '\n';
            continue;
        }

        const auto &reject = std::get<OrderReject>(report);
        output += reject.orderId;
        output += ',';
        output += reject.clientOrderId;
        output += ',';
        output += reject.instrument;
        output += ',';
        output += reject.side;
        output += ',';
        output += "Reject";
        output += ',';
        output += reject.quantity;
        output += ',';
        output += reject.price;
        output += ',';
        output += reject.reason;
        output += ',';
        output += reject.timestamp;
        output += '\n';
    }

    file.write(output.data(), static_cast<std::streamsize>(output.size()));
}
