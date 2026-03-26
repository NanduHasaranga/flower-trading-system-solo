#ifndef ORDER_HPP
#define ORDER_HPP

#include <string>
#include "Types.hpp"

struct Order
{
    std::string clientOrderId;
    std::string orderId;
    Instrument instrument;
    Side side;
    double price;
    int quantity;
};

#endif