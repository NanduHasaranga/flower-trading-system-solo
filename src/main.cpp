#include <fstream>
#include <iostream>
#include <iterator>
#include <variant>
#include <vector>

#include "Core/ExecutionReport.hpp"
#include "Core/OrderReject.hpp"
#include "Engine/Exchange.hpp"
#include "IO/CsvReader.hpp"
#include "IO/CsvWriter.hpp"
#include "IO/OrderProcessor.hpp"

using Record = std::variant<ExecutionReport, OrderReject>;

int main()
{
    std::vector<Record> records;
    Exchange exchange;
    CsvReader reader;
    CsvWriter writer;
    OrderProcessor processor;

    std::ifstream file("data/orders.csv");
    if (!file.is_open())
    {
        std::cerr << "Failed to open data/orders.csv\n";
        return 1;
    }

    reader.parseFile(file, [&](const std::vector<std::string> &row)
                     {
        if (row.empty()) return;

        auto result = processor.processRow(row);

        if (auto* order = std::get_if<Order>(&result)) {
            auto& orderBook = exchange.getOrderBook(*order);
            auto reports = orderBook.processOrder(*order);
            records.insert(records.end(), std::make_move_iterator(reports.begin()), std::make_move_iterator(reports.end()));
        } else {
            records.push_back(std::get<OrderReject>(result));
        } });

    writer.writeChronological("data/execution.csv", records);

    return 0;
}