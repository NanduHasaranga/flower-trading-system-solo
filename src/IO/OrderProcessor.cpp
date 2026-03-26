#include "IO/OrderProcessor.hpp"
#include "Utils/TimeUtils.hpp"
#include <cstdlib>
#include <stdexcept>

long OrderProcessor::nextOrderId = 1;

std::string OrderProcessor::generateOrderID() {
    return "ord" + std::to_string(nextOrderId++);
}

// Convert string to Instrument enum
Instrument OrderProcessor::instrumentFromString(const std::string& str) {
    if (str == "Rose") return Instrument::Rose;
    if (str == "Lavender") return Instrument::Lavender;
    if (str == "Lotus") return Instrument::Lotus;
    if (str == "Tulip") return Instrument::Tulip;
    if (str == "Orchid") return Instrument::Orchid;
    throw std::invalid_argument("Invalid Instrument");
}

// Convert string to Side enum
Side OrderProcessor::sideFromInt(const std::string& str) {
    if (str == "1") return Side::Buy;
    if (str == "2") return Side::Sell;
    throw std::invalid_argument("Invalid Side");
}

// Validate row according to your rules
bool OrderProcessor::validate(const std::vector<std::string>& row, std::string& reason) {
    if (row.size() < 5) {
        reason = "Missing required field(s)";
        return false;
    }

    try { instrumentFromString(row[1]); }
    catch (...) { reason = "Invalid Instrument"; return false; }

    try { sideFromInt(row[2]); }
    catch (...) { reason = "Invalid Side"; return false; }

    int quantity = std::atoi(row[3].c_str());
    if (quantity < 10 || quantity > 1000 || quantity % 10 != 0) {
        reason = "Invalid Quantity";
        return false;
    }

    double price = std::atof(row[4].c_str());
    if (price <= 0) {
        reason = "Invalid Price";
        return false;
    }

    return true;
}

// Process a CSV row
std::variant<Order, ExecutionReport> OrderProcessor::processRow(const std::vector<std::string>& row) {
    std::string reason;
    std::string orderId = generateOrderID();

    // Defaults for rejected
    Instrument inst = Instrument::Rose;
    Side side = Side::Buy;
    int quantity = 0;
    double price = 0.0;

    if (row.size() > 1) {
        try { inst = instrumentFromString(row[1]); } catch(...) {}
    }
    if (row.size() > 2) {
        try { side = sideFromInt(row[2]); } catch(...) {}
    }
    if (row.size() > 3 && !row[3].empty()) quantity = std::atoi(row[3].c_str());
    if (row.size() > 4 && !row[4].empty()) price = std::atof(row[4].c_str());

    if (!validate(row, reason)) {
        std::string clientId = row.size() > 0 ? row[0] : "";
        return ExecutionReport{
            clientId,
            orderId,
            row.size() > 1 ? row[1] : "",
            row.size() > 2 ? row[2] : "",
            row.size() > 4 ? row[4] : "",
            row.size() > 3 ? row[3] : "",
            "Reject",
            reason,
            utils::getCurrentTimestamp()  // transactionTime placeholder
        };
    }

    // Valid order
    Order order{
        row[0],       // clientOrderId
        orderId,      // orderId
        instrumentFromString(row[1]),
        sideFromInt(row[2]),
        price,
        quantity
    };
    return order;
}