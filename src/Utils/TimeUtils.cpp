#include "Utils/TimeUtils.hpp"
#include <chrono>
#include <cstdio>
#include <ctime>

namespace utils
{

    Timestamp getCurrentTimestamp()
    {
        thread_local Timestamp cached;
        thread_local long long lastMs = -1;

        auto now = std::chrono::system_clock::now();
        auto msEpoch = std::chrono::duration_cast<std::chrono::milliseconds>(
                           now.time_since_epoch())
                           .count();

        if (msEpoch != lastMs)
        {
            lastMs = msEpoch;
            std::time_t t = std::chrono::system_clock::to_time_t(now);
            std::tm localTime{};
            localtime_s(&localTime, &t);

            int written = std::snprintf(
                cached.buf,
                sizeof(cached.buf),
                "%04d%02d%02d-%02d%02d%02d.%03d",
                localTime.tm_year + 1900,
                localTime.tm_mon + 1,
                localTime.tm_mday,
                localTime.tm_hour,
                localTime.tm_min,
                localTime.tm_sec,
                static_cast<int>(msEpoch % 1000));
            cached.len = static_cast<unsigned char>(written);
        }

        return cached;
    }

}