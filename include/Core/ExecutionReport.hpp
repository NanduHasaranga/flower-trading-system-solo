#ifndef EXECUTIONREPORT_HPP
#define EXECUTIONREPORT_HPP

#include <string>
#include "Types.hpp"

struct ExecutionReport {
    std::string clientOrderId;
    std::string orderId;
    Instrument instrument;
    Side side;
    double price;
    int quantity;
    int status;
    std::string reason;
    std::string transactionTime;

    ExecutionReport() = default;

    ExecutionReport( 
        std::string clientOrderId, 
        std::string orderId, 
        Instrument instrument,  
        Side side, 
        double price, 
        int quantity, 
        OrderStatus status,  
        std::string reason, 
        std::string transactionTime) : clientOrderId(clientOrderId), orderId(orderId), instrument(instrument), side(side), price(price), quantity(quantity), reason(" "), transactionTime(" ") {}
};
#endif