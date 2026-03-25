#include "IO/CsvWriter.hpp"
#include "Core/Types.hpp"
#include <fstream>
#include <iomanip>

void CsvWriter::writeRejects(const std::string& path, const std::vector<OrderReject>& rejects)
{
    std::ofstream file(path);
    file << "clientOrderId,instrument,side,price,quantity,reason,timestamp\n";
    
    for (const auto& r : rejects)
    {
        file << r.clientOrderId << ","
             << r.instrument << ","
             << r.side << ","
             << r.price << ","
             << r.quantity << ","
             << r.reason << ","
             << r.timestamp << "\n";
    }
}

void CsvWriter::writeExecutions(const std::string& path, const std::vector<ExecutionReport>& reports)
{
    std::ofstream file(path);
    file << "clientOrderId,orderId,instrument,side,price,quantity,status,transactionTime\n";
    
    for (const auto& r : reports)
    {
        file << r.clientOrderId << ","
             << r.orderId << ","
             << to_string(r.instrument) << ","
             << to_string(r.side) << ","
             << std::fixed << std::setprecision(2) << r.price << ","
             << r.quantity << ","
             << to_string(r.status) << ","
             << r.transactionTime << "\n";
    }
}