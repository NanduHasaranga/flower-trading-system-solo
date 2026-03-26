#include "Utils/TimeUtils.hpp"
#include <chrono>
#include <cstdio>
#include <ctime>

namespace utils
{

    std::string getCurrentTimestamp()
    {
        auto now = std::chrono::system_clock::now();
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                      now.time_since_epoch()) %
                  1000;

        std::time_t t = std::chrono::system_clock::to_time_t(now);
        std::tm localTime{};
        localtime_s(&localTime, &t);

        char buffer[32];
        std::snprintf(
            buffer,
            sizeof(buffer),
            "%04d%02d%02d-%02d%02d%02d.%03d",
            localTime.tm_year + 1900,
            localTime.tm_mon + 1,
            localTime.tm_mday,
            localTime.tm_hour,
            localTime.tm_min,
            localTime.tm_sec,
            static_cast<int>(ms.count()));

        return std::string(buffer);
    }

}