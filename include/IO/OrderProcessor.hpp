#pragma once
#include <string>
#include <variant>
#include <vector>
#include "Core/Order.hpp"
#include "Core/OrderReject.hpp"

class OrderProcessor
{
private:
    static long nextOrderId;
    static std::string generateOrderID();

public:
    static std::variant<Order, OrderReject> processRow(const std::vector<std::string> &row);
};