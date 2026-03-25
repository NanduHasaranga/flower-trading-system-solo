#ifndef TYPES_HPP
#define TYPES_HPP

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

#endif