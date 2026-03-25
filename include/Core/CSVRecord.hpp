#ifndef CSVRECORD_HPP
#define CSVRECORD_HPP

#include <string>

struct CsvRecord
{
    std::string clientOrderId;
    std::string orderId;
    std::string instrument;
    std::string side;
    std::string price;
    std::string quantity;
    std::string status;
    std::string reason;
    std::string timestamp;
};

#endif