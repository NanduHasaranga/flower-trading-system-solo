#pragma once

#include <string>

struct RawOrder
{
    std::string clientOrderId;
    std::string instrument;
    std::string side;
    std::string price;
    std::string quantity;
};
