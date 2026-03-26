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

// void CsvWriter::writeExecutions(const std::string &path, const std::vector<ExecutionReport> &reports)
// {
//     std::ofstream file(path);
//     file << "Order ID,Client Order ID,Instrument,Side,Exec Status,Quantity,Price,Reason,Transaction Time\n";

//     for (const auto &r : reports)
//     {
//         file << r.orderId << ","
//              << r.clientOrderId << ","
//              << to_string(r.instrument) << ","
//              << static_cast<int>(r.side) << ","
//              << static_cast<int>(r.status) << ","
//              << r.quantity << ","
//              << std::fixed << std::setprecision(2) << r.price << ","
//              << ","
//              << r.transactionTime << "\n";
//     }
// }

// void CsvWriter::writeChronological(const std::string &path, const std::vector<Record> &records)
// {
//     std::ofstream file(path);
//     file << "Order ID,Client Order ID,Instrument,Side,Exec Status,Quantity,Price,Reason,Transaction Time\n";

//     for (const auto &record : records)
//     {
//         std::visit([&file](auto &&arg)
//                    {
//             using T = std::decay_t<decltype(arg)>;
//             if constexpr (std::is_same_v<T, ExecutionReport>)
//             {
//                 const auto& r = arg;
//                  file << r.orderId << ","
//                      << r.clientOrderId << ","
//                      << to_string(r.instrument) << ","
//                      << static_cast<int>(r.side) << ","
//                      << static_cast<int>(r.status) << ","
//                      << r.quantity << ","
//                      << std::fixed << std::setprecision(2) << r.price << ","
//                      << ","
//                      << r.transactionTime << "\n";
//             }
//             else if constexpr (std::is_same_v<T, OrderReject>)
//             {
//                 const auto& r = arg;
//                  file << ","
//                      << r.clientOrderId << ","
//                      << r.instrument << ","
//                      << r.side << ","
//                      << static_cast<int>(OrderStatus::Reject) << ","
//                      << r.quantity << ","
//                      << r.price << ","
//                      << r.reason << ","
//                      << r.timestamp << "\n";
//             } }, record);
//     }
// }