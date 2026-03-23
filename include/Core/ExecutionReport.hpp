#ifndef EXECUTIONREPORT_HPP
#define EXECUTIONREPORT_HPP

#include <string>
#include "Types.hpp"

struct ExecutionReport
{
    std::string clientOrderId;
    std::string orderId;
    Instrument instrument;
    Side side;
    double price;
    int quantity;
    int status;
    std::string reason;
    std::string transactionTime;

    
};

#endif