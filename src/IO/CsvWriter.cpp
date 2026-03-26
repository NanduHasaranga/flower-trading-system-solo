#include "IO/CsvWriter.hpp"
#include "Core/Types.hpp"
#include <fstream>
#include <iomanip>

void CsvWriter::writeExecutions(const std::string &path, const std::vector<ExecutionReport> &rejects)
{
    std::ofstream file(path);
    file << "Order ID,Cl. Ord.ID,Instrument,Side,Exec Status,Quantity,Price,reason\n";

    for (const auto &r : rejects)
    {
        file << r.orderId << ","
             << r.clientOrderId << ","
             << r.instrument << ","
             << r.side << ","
             << r.status << ","
             << r.quantity << ","
             << r.price << ","
             << r.reason << "\n";
    }
}
