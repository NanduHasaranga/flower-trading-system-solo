#include "Utils/TimeUtils.hpp"
#include <chrono>
#include <iomanip>
#include <sstream>

namespace utils
{

    std::string getCurrentTimestamp()
    {
        auto now = std::chrono::system_clock::now();
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                      now.time_since_epoch()) %
                  1000;

        std::time_t t = std::chrono::system_clock::to_time_t(now);
        std::tm *localTime = std::localtime(&t);

        std::ostringstream oss;
        oss << std::put_time(localTime, "%Y%m%d-%H%M%S")
            << '.' << std::setfill('0') << std::setw(3) << ms.count();

        return oss.str();
    }

}