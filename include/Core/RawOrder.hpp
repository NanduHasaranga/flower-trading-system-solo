#ifndef RAWORDER_HPP
#define RAWORDER_HPP

#include <string>

struct RawOrder
{
    std::string clientOrderId;
    std::string instrument;
    std::string side;
    std::string price;
    std::string quantity;
};

#endif