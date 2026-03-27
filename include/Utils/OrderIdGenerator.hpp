#pragma once

#include <string>
#include <atomic>

namespace Utils
{
    class OrderIdGenerator
    {
    public:
        static std::string generateId();

    private:
        static std::atomic<long> nextOrderId;
    };

}