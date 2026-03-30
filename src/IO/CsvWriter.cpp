#include "IO/CsvWriter.hpp"
#include "Core/Types.hpp"
#include <charconv>
#include <cstring>
#include <fstream>
#include <vector>

namespace
{
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

    struct LineBuilder
    {
        char buf[512];
        char *p = buf;

        void append(std::string_view sv)
        {
            std::memcpy(p, sv.data(), sv.size());
            p += sv.size();
        }

        void append(char c) { *p++ = c; }

        void appendInt(int value)
        {
            auto [end, ec] = std::to_chars(p, p + 16, value);
            p = end;
        }

        void appendDouble(double value)
        {
            auto [end, ec] = std::to_chars(p, p + 32, value);
            p = end;
        }

        std::size_t size() const { return static_cast<std::size_t>(p - buf); }
    };
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
        LineBuilder line;

        if (const auto *execution = std::get_if<ExecutionReport>(&report))
        {
            line.append(execution->orderId);
            line.append(',');
            line.append(execution->clientOrderId);
            line.append(',');
            line.append(to_string(execution->instrument));
            line.append(',');
            line.append(to_string(execution->side));
            line.append(',');
            line.append(statusToString(execution->status));
            line.append(',');
            line.appendInt(execution->quantity);
            line.append(',');
            line.appendDouble(execution->price);
            line.append(',');
            line.append(',');
            line.append(execution->transactionTime);
            line.append('\n');
        }
        else
        {
            const auto &reject = std::get<OrderReject>(report);
            line.append(reject.orderId);
            line.append(',');
            line.append(reject.clientOrderId);
            line.append(',');
            line.append(reject.instrument);
            line.append(',');
            line.append(reject.side);
            line.append(',');
            line.append("Reject");
            line.append(',');
            line.append(reject.quantity);
            line.append(',');
            line.append(reject.price);
            line.append(',');
            line.append(reject.reason);
            line.append(',');
            line.append(reject.timestamp);
            line.append('\n');
        }

        output.append(line.buf, line.size());
    }

    file.write(output.data(), static_cast<std::streamsize>(output.size()));
}
