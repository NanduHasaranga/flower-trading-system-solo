#include <gtest/gtest.h>
#include "Engine/Exchange.hpp"
#include "Core/Order.hpp"
#include <string_view>

namespace
{
    Order makeOrder(Instrument inst)
    {
        return Order{
            FixedString<16>("cli"),
            FixedString<16>("ord"),
            inst, Side::Buy, 10.0, 100};
    }
}

TEST(ExchangeTest, ReturnsDistinctBooksForDifferentInstruments)
{
    Exchange exchange;

    auto rose = makeOrder(Instrument::Rose);
    auto tulip = makeOrder(Instrument::Tulip);

    OrderBook &bookRose = exchange.getOrderBook(rose);
    OrderBook &bookTulip = exchange.getOrderBook(tulip);

    EXPECT_NE(&bookRose, &bookTulip);
}

TEST(ExchangeTest, ReturnsSameBookForSameInstrument)
{
    Exchange exchange;

    auto o1 = makeOrder(Instrument::Lavender);
    auto o2 = makeOrder(Instrument::Lavender);

    EXPECT_EQ(&exchange.getOrderBook(o1), &exchange.getOrderBook(o2));
}

TEST(ExchangeTest, AllFiveInstrumentsHaveDistinctBooks)
{
    Exchange exchange;

    auto rose = makeOrder(Instrument::Rose);
    auto lavender = makeOrder(Instrument::Lavender);
    auto lotus = makeOrder(Instrument::Lotus);
    auto tulip = makeOrder(Instrument::Tulip);
    auto orchid = makeOrder(Instrument::Orchid);

    std::set<OrderBook *> books;
    books.insert(&exchange.getOrderBook(rose));
    books.insert(&exchange.getOrderBook(lavender));
    books.insert(&exchange.getOrderBook(lotus));
    books.insert(&exchange.getOrderBook(tulip));
    books.insert(&exchange.getOrderBook(orchid));

    EXPECT_EQ(books.size(), 5u);
}
