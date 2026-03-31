#pragma once

#include <string>
#include "Types.hpp"
#include "Utils/FixedString.hpp"

struct Order
{
    FixedString<16> clientOrderId;
    FixedString<16> orderId;
    Instrument instrument;
    Side side;
    double price;
    int quantity;
};