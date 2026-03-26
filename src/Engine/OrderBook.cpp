#include "Engine\OrderBook.hpp"

#include <iostream> 
// long OrderBook::nextOrderId = 1;

// std::string OrderBook::generateOrderID() {
//     return "ord" + toString(nextOrderId++);
// }


bool OrderBook::isMatchingOrder(const Order &order) {

    if(order.side == Side::Sell && !OrderBook::buyingSide.isEmpty() && OrderBook::buyingSide.getBestOrder().price >= order.price && order.quantity > 0) {
        return true;
    }else if (order.side == Side::Buy && !OrderBook::sellingSide.isEmpty() && OrderBook::sellingSide.getBestOrder().price <= order.price && order.quantity > 0) {
        return true;
    }else
        return false;
}

std::vector<ExecutionReport> OrderBook::processOrder(Order &order) {
    std::vector<ExecutionReport> reports;
    //order.orderId = generateOrderID();    

    bool isProceed = false;

    if(order.side == Side::Sell) {    
        while(isMatchingOrder(order)) {        
            Order topOrder = OrderBook::buyingSide.getBestOrder();
            if(order.quantity >= topOrder.quantity) {
                int proceedQuantity = topOrder.quantity;
                if(order.quantity == topOrder.quantity){
                    reports.push_back(ExecutionReport(order.clientOrderId, order.orderId, to_string(order.instrument), to_string(order.side), toString(topOrder.price), toString(proceedQuantity), "Fill" , " ", " "));
                    reports.push_back(ExecutionReport(topOrder.clientOrderId, topOrder.orderId, to_string(topOrder.instrument), to_string(topOrder.side), toString(topOrder.price), toString(proceedQuantity), "Fill", " ", " "));
                    OrderBook::buyingSide.removeTopOrder();
                    return reports;
                }else {
                    reports.push_back(ExecutionReport(order.clientOrderId, order.orderId, to_string(order.instrument), to_string(order.side), toString(topOrder.price), toString(proceedQuantity), "PFill", " ", " "));
                    reports.push_back(ExecutionReport(topOrder.clientOrderId, topOrder.orderId, to_string(topOrder.instrument), to_string(topOrder.side), toString(topOrder.price), toString(proceedQuantity), "Fill", " ", " "));
                    order.quantity = order.quantity - proceedQuantity;
                    OrderBook::buyingSide.removeTopOrder();
                    isProceed = true;
                }
            }else {
                int proceedQuantity = order.quantity;
                reports.push_back(ExecutionReport(order.clientOrderId, order.orderId, to_string(order.instrument), to_string(order.side), toString(topOrder.price), toString(proceedQuantity), "Fill", " ", " "));
                reports.push_back(ExecutionReport(topOrder.clientOrderId, topOrder.orderId, to_string(topOrder.instrument), to_string(topOrder.side), toString(topOrder.price), toString(proceedQuantity), "PFill", " ", " "));
                OrderBook::buyingSide.updateTopOrderQuantity(topOrder.quantity - proceedQuantity);
                return reports;
            }
        }
        if(!isProceed)
            reports.push_back(ExecutionReport(order.clientOrderId, order.orderId, to_string(order.instrument), to_string(order.side), toString(order.price), toString(order.quantity), "New", " ", " "));
        OrderBook::sellingSide.insertOrder(order);
        return reports;
    }else {
        while(isMatchingOrder(order)) {
             Order topOrder = OrderBook::sellingSide.getBestOrder();
             if(order.quantity >= topOrder.quantity) {
                int proceedQuantity = topOrder.quantity;
                if(order.quantity == topOrder.quantity){
                    reports.push_back(ExecutionReport(order.clientOrderId, order.orderId, to_string(order.instrument), to_string(order.side), toString(topOrder.price), toString(proceedQuantity), "Fill" , " ", " "));
                    reports.push_back(ExecutionReport(topOrder.clientOrderId, topOrder.orderId, to_string(topOrder.instrument), to_string(topOrder.side), toString(topOrder.price), toString(proceedQuantity), "Fill", " ", " "));
                    OrderBook::sellingSide.removeTopOrder();
                    return reports;
                }else {
                    reports.push_back(ExecutionReport(order.clientOrderId, order.orderId, to_string(order.instrument), to_string(order.side), toString(topOrder.price), toString(proceedQuantity), "PFill", " ", " "));
                     reports.push_back(ExecutionReport(topOrder.clientOrderId, topOrder.orderId, to_string(topOrder.instrument), to_string(topOrder.side), toString(topOrder.price), toString(proceedQuantity), "Fill", " ", " "));
                     order.quantity = order.quantity - proceedQuantity;
                     OrderBook::sellingSide.removeTopOrder();
                     isProceed = true;
                }
            }else {
                int proceedQuantity = order.quantity;
                reports.push_back(ExecutionReport(order.clientOrderId, order.orderId, to_string(order.instrument), to_string(order.side), toString(topOrder.price), toString(proceedQuantity), "Fill", " ", " "));
                reports.push_back(ExecutionReport(topOrder.clientOrderId, topOrder.orderId, to_string(topOrder.instrument), to_string(topOrder.side), toString(topOrder.price), toString(proceedQuantity), "PFill", " ", " "));
                OrderBook::sellingSide.updateTopOrderQuantity(topOrder.quantity - proceedQuantity);
                return reports;
            }
        }
        if(!isProceed)
            reports.push_back(ExecutionReport(order.clientOrderId, order.orderId, to_string(order.instrument), to_string(order.side), toString(order.price), toString(order.quantity), "New", " ", " "));
        OrderBook::buyingSide.insertOrder(order);
        return reports;
    }    
}