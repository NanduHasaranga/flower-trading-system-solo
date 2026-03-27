#pragma once

#include <array>

enum class Side
{
    Buy = 0,
    Sell = 1
};

enum class Instrument
{
    Rose,
    Lavender,
    Lotus,
    Tulip,
    Orchid
};

enum class OrderStatus
{
    New,
    Fill,
    PFill,
    Reject
};

constexpr std::array<const char *, 5> InstrumentStrings = {
    "Rose", "Lavender", "Lotus", "Tulip", "Orchid"};

constexpr std::array<const char *, 2> SideStrings = {
    "1", "2"};

inline constexpr const char *to_string(Instrument i)
{
    return InstrumentStrings[static_cast<int>(i)];
}

inline constexpr const char *to_string(Side s)
{
    return SideStrings[static_cast<int>(s)];
}