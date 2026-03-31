#include "Utils/OrderIdGenerator.hpp"
#include <charconv>

namespace Utils
{

    std::atomic<long> OrderIdGenerator::nextOrderId{1};

    std::string OrderIdGenerator::generateId()
    {
        char buffer[16] = {'o', 'r', 'd'};
        long id = nextOrderId.fetch_add(1, std::memory_order_relaxed);
        auto [ptr, ec] = std::to_chars(buffer + 3, buffer + sizeof(buffer), id);
        return std::string(buffer, static_cast<std::size_t>(ptr - buffer));
    }

    void OrderIdGenerator::reset(long startId)
    {
        nextOrderId.store(startId, std::memory_order_relaxed);
    }

}