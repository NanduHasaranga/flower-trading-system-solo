#pragma once

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