#include "IO/CsvWriter.hpp"
#include "Core/Types.hpp"
#include <fstream>
#include <iomanip>
#include <vector>

void CsvWriter::writeExecutions(const std::string &path, const std::vector<ExecutionReport> &rejects)
{
    std::ofstream file(path);
    std::vector<char> ioBuffer(1 << 20);
    file.rdbuf()->pubsetbuf(ioBuffer.data(), static_cast<std::streamsize>(ioBuffer.size()));

    std::string output;
    output.reserve(80 + rejects.size() * 128);
    output += "Order ID,Cl. Ord.ID,Instrument,Side,Exec Status,Quantity,Price,reason,Transaction Time\n";

    for (const auto &r : rejects)
    {
        output += r.orderId;
        output += ',';
        output += r.clientOrderId;
        output += ',';
        output += r.instrument;
        output += ',';
        output += r.side;
        output += ',';
        output += r.status;
        output += ',';
        output += r.quantity;
        output += ',';
        output += r.price;
        output += ',';
        output += r.reason;
        output += ',';
        output += r.transactionTime;
        output += '\n';
    }

    file.write(output.data(), static_cast<std::streamsize>(output.size()));
}
