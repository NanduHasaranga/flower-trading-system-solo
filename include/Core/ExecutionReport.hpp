#ifndef EXECUTIONREPORT_HPP
#define EXECUTIONREPORT_HPP

#include <string>

struct ExecutionReport {
    std::string clientOrderId;
    std::string orderId;
    std::string instrument;
    std::string side;
    std::string price;
    std::string quantity;
    std::string status;
    std::string reason;
    std::string transactionTime;

    ExecutionReport() = default;

    ExecutionReport( 
        std::string clientOrderId, 
        std::string orderId, 
        std::string instrument,  
        std::string side, 
        std::string price, 
        std::string quantity, 
        std::string status,  
        std::string reason, 
        std::string transactionTime) : clientOrderId(clientOrderId), orderId(orderId), instrument(instrument), side(side), price(price), quantity(quantity), status(status), reason(reason), transactionTime(transactionTime) {}
};
#endif