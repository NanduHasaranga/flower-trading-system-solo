#include <iostream>
#include "Engine\Exchange.hpp"
#include "IO\CsvReader.hpp"
#include "IO\OrderProcessor.hpp"
#include "IO\CsvWriter.hpp"

int main()
{
    std::vector<ExecutionReport> records;
    Exchange exchange;
    CsvWriter writer;
    CsvReader reader;

    if (!reader.open("data/orders.csv"))
    {
        return 1;
    }

    while (auto row = reader.nextRow())
    {
        auto result = OrderProcessor::processRow(*row);

        if (auto *order = std::get_if<Order>(&result))
        {
            auto &orderBook = exchange.getOrderBook(*order);
            orderBook.processOrder(*order, records);
        }
        else
        {
            records.push_back(std::get<ExecutionReport>(result));
        }
    }

    reader.close();
    writer.writeExecutions("data/execution.csv", records);
}