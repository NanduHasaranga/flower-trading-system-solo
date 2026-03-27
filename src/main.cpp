#include <iostream>
#include <algorithm>
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
        std::vector<std::string> fields;
        const std::size_t bound = std::min(row->fieldCount, row->fields.size());
        fields.reserve(bound);
        for (std::size_t i = 0; i < bound; ++i)
        {
            fields.emplace_back(row->fields[i]);
        }

        auto result = OrderProcessor::processRow(fields);

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