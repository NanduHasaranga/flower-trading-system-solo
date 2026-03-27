#include "IO/CsvWriter.hpp"
#include "Core/Types.hpp"
#include <charconv>
#include <cstdio>
#include <memory>

namespace
{
    constexpr std::size_t kFileBufferSize = 8 << 20; // 8 MiB buffer

    inline std::string_view to_string_view(OrderStatus status)
    {
        switch (status)
        {
        case OrderStatus::New:
            return "New";
        case OrderStatus::Reject:
            return "Reject";
        case OrderStatus::Fill:
            return "Fill";
        case OrderStatus::PFill:
            return "PFill";
        }
        return "Unknown";
    }

    inline void appendString(std::string &buffer, std::string_view str)
    {
        buffer.append(str);
    }

    inline void appendInt(std::string &buffer, int value)
    {
        char buf[24];
        const auto [ptr, ec] = std::to_chars(buf, buf + sizeof(buf), value);
        if (ec == std::errc())
            buffer.append(buf, static_cast<std::size_t>(ptr - buf));
    }

    inline void appendDouble2(std::string &buffer, double value)
    {
        char buf[64];
        const int len = std::snprintf(buf, sizeof(buf), "%.2f", value);
        if (len > 0)
            buffer.append(buf, static_cast<std::size_t>(len));
    }

    inline void appendChar(std::string &buffer, char c)
    {
        buffer.push_back(c);
    }
}

void CsvWriter::writeHeader(FILE *file)
{
    static constexpr char kHeader[] =
        "Order ID,Client Order ID,Instrument,Side,Exec Status,Quantity,Price,Reason,Transaction Time\n";
    std::fwrite(kHeader, 1, sizeof(kHeader) - 1, file);
}

void CsvWriter::writeExecutionRow(FILE *file, const ExecutionReport &r, std::string &rowBuffer)
{
    rowBuffer.clear();
    rowBuffer.reserve(160);

    appendString(rowBuffer, r.orderId);
    appendChar(rowBuffer, ',');
    appendString(rowBuffer, r.clientOrderId);
    appendChar(rowBuffer, ',');
    appendString(rowBuffer, to_string(r.instrument));
    appendChar(rowBuffer, ',');
    appendInt(rowBuffer, static_cast<int>(r.side));
    appendChar(rowBuffer, ',');
    appendString(rowBuffer, to_string_view(r.status));
    appendChar(rowBuffer, ',');
    appendInt(rowBuffer, r.quantity);
    appendChar(rowBuffer, ',');
    appendDouble2(rowBuffer, r.price);
    appendChar(rowBuffer, ','); // empty Reason
    appendChar(rowBuffer, ',');
    appendString(rowBuffer, r.transactionTime);
    appendChar(rowBuffer, '\n');

    std::fwrite(rowBuffer.data(), 1, rowBuffer.size(), file);
}

void CsvWriter::writeRejectRow(FILE *file, const OrderReject &r, std::string &rowBuffer)
{
    rowBuffer.clear();
    rowBuffer.reserve(192);

    appendString(rowBuffer, r.orderId);
    appendChar(rowBuffer, ',');
    appendString(rowBuffer, r.clientOrderId);
    appendChar(rowBuffer, ',');
    appendString(rowBuffer, r.instrument);
    appendChar(rowBuffer, ',');
    appendString(rowBuffer, r.side);
    appendChar(rowBuffer, ',');
    appendString(rowBuffer, "Reject");
    appendChar(rowBuffer, ',');
    appendString(rowBuffer, r.quantity);
    appendChar(rowBuffer, ',');
    appendString(rowBuffer, r.price);
    appendChar(rowBuffer, ',');
    appendString(rowBuffer, r.reason);
    appendChar(rowBuffer, ',');
    appendString(rowBuffer, r.timestamp);
    appendChar(rowBuffer, '\n');

    std::fwrite(rowBuffer.data(), 1, rowBuffer.size(), file);
}

void CsvWriter::writeChronological(const std::string &path, const std::vector<Record> &records)
{
    // The stdio buffer must outlive FILE* close when using setvbuf.
    std::vector<char> buffer(kFileBufferSize);

    std::unique_ptr<FILE, decltype(&std::fclose)> file(
        std::fopen(path.c_str(), "wb"), &std::fclose);

    if (!file)
        return;

    std::setvbuf(file.get(), buffer.data(), _IOFBF, kFileBufferSize);

    writeHeader(file.get());

    std::string rowBuffer;
    rowBuffer.reserve(192);

    for (const auto &record : records)
    {
        if (const auto *execution = std::get_if<ExecutionReport>(&record))
            writeExecutionRow(file.get(), *execution, rowBuffer);
        else
            writeRejectRow(file.get(), std::get<OrderReject>(record), rowBuffer);
    }
}