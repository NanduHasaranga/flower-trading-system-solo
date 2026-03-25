#include <iostream>
#include <variant>
#include <vector>
#include <fstream>
#include "Engine/Exchange.hpp"
#include "IO/CsvReader.hpp"
#include "IO/OrderProcessor.hpp"
#include "IO/CsvWriter.hpp"
#include "Core/ExecutionReport.hpp"
#include "Core/OrderReject.hpp"

using Record = std::variant<ExecutionReport, OrderReject>;

int main()
{
    std::vector<Record> records;
    Exchange exchange;
    CsvReader reader;
    CsvWriter writer;

    std::ifstream file("data/orders.csv");
    auto rows = reader.parseFile(file);

    for (const auto& row : rows)
    {
        if (row.empty())
            continue;

        auto result = OrderProcessor::processRow(row);

        if (std::holds_alternative<Order>(result))
        {
            auto order = std::get<Order>(result);
            auto& orderBook = exchange.getOrderBook(order);
            auto reports = orderBook.processOrder(order);

            for (const auto& r : reports)
            {
                records.push_back(r);
            }
        }
        else
        {
            records.push_back(std::get<OrderReject>(result));
        }
    }

    writer.writeChronological("data/output.csv", records);

    return 0;
}