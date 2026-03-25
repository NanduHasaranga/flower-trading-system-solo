#ifndef ORDERPROCESSOR_HPP
#define ORDERPROCESSOR_HPP

#include <string>
#include <vector>
#include <variant>
#include "Core/Types.hpp"
#include "Core/ExecutionReport.hpp"
#include "Core/OrderReject.hpp"
#include "Core/Order.hpp"

class OrderProcessor
{
private:
    static long nextOrderId;
    static std::string generateOrderID();
    static Order createOrder(const RawOrder &raw, const std::string &orderId);

public:
    static std::variant<Order, OrderReject> processRow(const std::vector<std::string> &row);
};

#endif