#ifndef TYPES_HPP
#define TYPES_HPP

#include <array>

enum class Side
{
    Buy = 1,
    Sell = 2
};

enum class OrderStatus
{
    New = 0,
    Reject = 1,
    Fill = 2,
    PFill = 3
};

enum class Instrument
{
    Rose,
    Lavender,
    Lotus,
    Tulip,
    Orchid
};

constexpr std::array<const char *, 5> InstrumentStrings = {
    "Rose", "Lavender", "Lotus", "Tulip", "Orchid"};

// Enum → string (fast O(1) lookup)
inline constexpr const char *to_string(Instrument i)
{
    return InstrumentStrings[static_cast<int>(i)];
}

#endif
