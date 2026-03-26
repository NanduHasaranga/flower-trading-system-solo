#include <iostream>
#include "Engine\Exchange.hpp"
#include "IO\CsvReader.hpp"
#include "IO\OrderProcessor.hpp"
#include "IO\CsvWriter.hpp"
#include <fstream>

int main() {

    // std::vector<ExecutionReport> report;
    // std::vector<std::vector<ExecutionReport>> reports;

    // Order ord1{"aa13","", Instrument::Rose, Side::Buy, 55.00, 100};
    // Order ord2{"aa14","", Instrument::Rose, Side::Buy, 65.00, 100};
    // Order ord3{"aa15","", Instrument::Rose, Side::Sell, 1.00, 300};
    // //Order ord4{"aa16","", Instrument::Rose, Side::Buy, 2.00, 100};


    // Exchange exchange = Exchange();
    // auto& orderBook1 = exchange.getOrderBook(ord1);
    // report = orderBook1.processOrder(ord1);
    // // std::cout << &orderBook1 << std::endl;
    // reports.push_back(report);

    // auto& orderBook2 = exchange.getOrderBook(ord2);
    // report = orderBook2.processOrder(ord2);
    // // std::cout << &orderBook2 << std::endl;
    // reports.push_back(report);

    // auto& orderBook3 = exchange.getOrderBook(ord3);
    // report = orderBook3.processOrder(ord3);
    // // std::cout << &orderBook2 << std::endl;
    // reports.push_back(report);

    // // auto& orderBook4 = exchange.getOrderBook(ord4);
    // // report = orderBook4.processOrder(ord4);
    // // // std::cout << &orderBook2 << std::endl;
    // // reports.push_back(report);

    // for (int i = 0; i < reports.size(); i++) {
    //     for (int j = 0; j < reports.at(i).size(); j++) {
    //     std::cout << reports.at(i).at(j).clientOrderId << " ";
    //     std::cout << reports.at(i).at(j).orderId << " ";
    //     //std::cout << to_string(reports.at(i).at(j).side) << " ";
    //     std::cout << reports.at(i).at(j).price << " ";
    //     std::cout << reports.at(i).at(j).quantity << " ";
    //     std::cout << to_string(reports.at(i).at(j).instrument) << " ";
    //     std::cout << to_string(reports.at(i).at(j).status) << " ";
    //     //std::cout << to_string(st) << " ";
    //     std::cout << std::endl;
    //     }
    // }

    //std::vector<std::string> row = {"aa13", "Rose", "1", "100", "55.00"};
   // std::vector<std::string> row1 = {"aa14", "Rose", "2", "100", "56.00"};


    std::vector<ExecutionReport> records;
    Exchange exchange;
    CsvWriter writer;
    CsvReader reader;

    std::ifstream file("data/orders.csv");

    std::string line;

    // Skip header
    std::getline(file, line);

    while(std::getline(file, line)){

        auto row = reader.parseLine(line);

        std::cout << row[0] << std::endl;

        auto result = OrderProcessor::processRow(row);
        //auto result1 = OrderProcessor::processRow(row1);

        if (std::holds_alternative<Order>(result)) {
            std::vector<ExecutionReport> report;
            
            auto ord = std::get<Order>(result);
            
            auto& orderBook = exchange.getOrderBook(ord);
            report = orderBook.processOrder(ord);
            for (const auto& r : report)
            {
                records.push_back(r);
            }

            std::cout << "Valid order: " << ord.orderId << " Price: " << ord.price << "\n";
        } else {
            records.push_back(std::get<ExecutionReport>(result));
            //std::cout << "Rejected order: " << report.orderId << " Reason: " << report.reason << "\n";
        }  
    }
    
    for (int i = 0; i < records.size(); i++) {
            std::cout << records.at(i).orderId << " ";
            std::cout << records.at(i).clientOrderId << " ";
            std::cout << records.at(i).instrument << " ";
            std::cout << records.at(i).side << " ";
            std::cout << records.at(i).status << " ";
            std::cout << records.at(i).quantity << " ";
            std::cout << records.at(i).price << " ";
            std::cout << records.at(i).reason << " ";
            std::cout << std::endl;
    }

    writer.writeExecutions("data/execution.csv", records);

}