#pragma once

#include <string>
#include <atomic>

namespace Utils
{
    class OrderIdGenerator
    {
    public:
        static std::string generateId();
        static void reset(long startId = 1);

    private:
        static std::atomic<long> nextOrderId;
    };

}