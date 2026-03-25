#pragma once
#include <string>
#include <vector>
#include <variant>
#include "Core/Types.hpp"
#include "Core/ExecutionReport.hpp" 
#include "Core/Order.hpp"

class OrderProcessor {
private:
    
    static long nextOrderId;
    static std::string generateOrderID();
    static bool validate(const std::vector<std::string>& row, std::string& reason);
    static Instrument instrumentFromString(const std::string& str);
    static Side sideFromInt(const std::string& str);

public:
    static std::variant<Order, ExecutionReport> processRow(const std::vector<std::string>& row);
};