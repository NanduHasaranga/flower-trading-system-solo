#include <fstream>
#include <iostream>
#include <iterator>
#include <variant>
#include <vector>

#include "Engine/Exchange.hpp"
#include "IO/CsvReader.hpp"
#include "IO/CsvWriter.hpp"
#include "IO/OrderProcessor.hpp"

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
            orderBook.processOrder(*order, records);
        } else {
            records.push_back(std::get<OrderReject>(result));
        } });

    writer.writeChronological("data/execution.csv", records);

    return 0;
}