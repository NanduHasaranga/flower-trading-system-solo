#pragma once

#include <cstring>
#include <string_view>
#include <algorithm>
#include <charconv>

template <std::size_t Capacity>
struct FixedString
{
    char data_[Capacity]{};
    unsigned char len_ = 0;

    FixedString() = default;

    FixedString(std::string_view sv) noexcept
    {
        len_ = static_cast<unsigned char>(std::min(sv.size(), Capacity - 1));
        std::memcpy(data_, sv.data(), len_);
    }

    FixedString(const char *s) noexcept : FixedString(std::string_view(s)) {}

    operator std::string_view() const noexcept { return {data_, len_}; }

    const char *data() const noexcept { return data_; }
    std::size_t size() const noexcept { return len_; }
    bool empty() const noexcept { return len_ == 0; }
};
