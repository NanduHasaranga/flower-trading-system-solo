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
    static bool validate(const std::string& instrumentText,
                         const std::string& sideText,
                         const std::string& quantityText,
                         const std::string& priceText,
                         std::string& reason,
                         Instrument& instrument,
                         Side& side,
                         int& quantity,
                         double& price);

public:
    static std::variant<Order, ExecutionReport> processRow(const std::vector<std::string>& row);
};