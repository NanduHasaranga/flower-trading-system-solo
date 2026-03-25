#ifndef REJECTIONREPORT_HPP
#define REJECTIONREPORT_HPP

#include <string>

struct OrderReject
{
    std::string clientOrderId;
    std::string instrument;
    std::string side;
    std::string price;
    std::string quantity;
    std::string reason;
    std::string timestamp;

    OrderReject() = default;
};

#endif