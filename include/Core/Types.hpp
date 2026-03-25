#ifndef TYPES_HPP
#define TYPES_HPP

#include <string>
#include <array>
#include <unordered_map>
#include <stdexcept>

enum class Side {
    Buy = 1,
    Sell = 2
};

enum class OrderStatus {
    New = 0,
    Reject = 1,
    Fill = 2,
    PFill = 3

};

enum class Instrument {
    Rose,
    Lavender,
    Lotus,
    Tulip,
    Orchid
};

constexpr std::array<const char*, 5> InstrumentStrings = {
    "Rose", "Lavender", "Lotus", "Tulip", "Orchid"
};

constexpr std::array<const char*, 2> SideStrings = {
    "Buy", "Sell"
};

constexpr std::array<const char*, 4> OrderStatusStrings = {
    "New", "Reject", "Fill", "PFill"
};

// Enum → string (fast O(1) lookup)
inline constexpr const char* to_string(Instrument i) {
    return InstrumentStrings[static_cast<int>(i)];
}

inline constexpr const char* to_string(Side s) {
    return SideStrings[static_cast<int>(s)];
}

inline constexpr const char* to_string(OrderStatus o) {
    return OrderStatusStrings[static_cast<int>(o)];
}


#endif 