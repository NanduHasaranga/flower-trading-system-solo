#include <iostream>
#include <vector>
#include <variant>
#include "Engine\Exchange.hpp"
#include "IO\CsvReader.hpp"
#include "IO\OrderProcessor.hpp"
#include "IO\CsvWriter.hpp"
#include "Core\OrderReject.hpp"

int main()
{
    std::vector<std::variant<ExecutionReport, OrderReject>> records;
    Exchange exchange;
    CsvWriter writer;
    CsvReader reader("data/orders.csv");

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
            records.emplace_back(std::get<OrderReject>(result));
        }
    }

    writer.writeExecutions("data/execution.csv", records);
    return 0;
}