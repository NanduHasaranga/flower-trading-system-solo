#include <iostream>
#include <vector>
#include <variant>
#include "Engine/Exchange.hpp"
#include "IO/CsvReader.hpp"
#include "IO/OrderProcessor.hpp"
#include "IO/CsvWriter.hpp"
#include "Core/OrderReject.hpp"

static constexpr const char *kDefaultInputPath = "data/orders.csv";
static constexpr const char *kDefaultOutputPath = "data/execution.csv";

int main(int argc, char *argv[])
{
    const char *inputPath = (argc > 1) ? argv[1] : kDefaultInputPath;
    const char *outputPath = (argc > 2) ? argv[2] : kDefaultOutputPath;

    std::vector<std::variant<ExecutionReport, OrderReject>> records;
    Exchange exchange;
    CsvWriter writer;
    CsvReader reader(inputPath);

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

    writer.writeExecutions(outputPath, records);
    return 0;
}