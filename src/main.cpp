#include <iostream>
#include "Engine\Exchange.hpp"
#include "IO\CsvReader.hpp"
#include "IO\OrderProcessor.hpp"
#include "IO\CsvWriter.hpp"
#include <fstream>

int main()
{
    std::vector<ExecutionReport> records;
    Exchange exchange;
    CsvWriter writer;
    CsvReader reader;

    std::ifstream file("data/orders.csv");
    if (!file.is_open())
    {
        std::cerr << "Failed to open data/orders.csv\n";
        return 1;
    }

    std::string line;
    std::getline(file, line);

    while (std::getline(file, line))
    {
        auto row = reader.parseLine(line);
        if (row.empty())
            continue;

        auto result = OrderProcessor::processRow(row);

        if (auto *order = std::get_if<Order>(&result))
        {
            auto &orderBook = exchange.getOrderBook(*order);
            auto reports = orderBook.processOrder(*order);

            records.insert(records.end(), reports.begin(), reports.end());
        }
        else
        {
            records.push_back(std::get<ExecutionReport>(result));
        }
    }

    writer.writeExecutions("data/execution.csv", records);
}