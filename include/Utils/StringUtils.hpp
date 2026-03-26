#ifndef STRING_UTILS_HPP
#define STRING_UTILS_HPP

#include <string>
#include <charconv>

namespace utils{ 

    inline std::string toString(double v) noexcept{
        char b[32];
        auto r = std::to_chars(b, b + 32, v);
        return std::string(b, r.ptr);
    }

    inline std::string toString(int v) noexcept{
        char b[16];
        auto r = std::to_chars(b, b + 16, v);
        return std::string(b, r.ptr);
    }

}
#endif