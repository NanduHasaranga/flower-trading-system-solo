#pragma once

#include <cstdint>
#include <string_view>

namespace utils
{
    //"YYYYMMDD-HHMMSS.mmm"
    struct Timestamp
    {
        char buf[24]{};
        unsigned char len = 0;

        Timestamp() = default;

        operator std::string_view() const noexcept { return {buf, len}; }

        const char *data() const noexcept { return buf; }
        std::size_t size() const noexcept { return len; }
    };

    Timestamp getCurrentTimestamp();
}