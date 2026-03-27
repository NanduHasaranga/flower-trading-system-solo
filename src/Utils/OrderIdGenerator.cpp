#include "Utils/OrderIdGenerator.hpp"

namespace Utils
{

    std::atomic<long> OrderIdGenerator::nextOrderId{1};

    std::string OrderIdGenerator::generateId()
    {
        return "ord" + std::to_string(nextOrderId.fetch_add(1, std::memory_order_relaxed));
    }

}