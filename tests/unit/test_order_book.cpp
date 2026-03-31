#include <gtest/gtest.h>
#include "Engine/OrderBook.hpp"
#include "Core/ExecutionReport.hpp"
#include "Core/OrderReject.hpp"
#include "test_helpers.hpp"
#include <variant>
#include <vector>
#include <string_view>

namespace
{
    using Reports = std::vector<std::variant<ExecutionReport, OrderReject>>;

    Order makeOrder(const char *clientId, const char *orderId,
                    Instrument inst, Side side, double price, int qty)
    {
        return Order{
            FixedString<16>(clientId),
            FixedString<16>(orderId),
            inst, side, price, qty};
    }

    const ExecutionReport &execAt(const Reports &r, std::size_t i)
    {
        return std::get<ExecutionReport>(r.at(i));
    }

    void expectExec(const ExecutionReport &e,
                    std::string_view orderId, std::string_view clientId,
                    Side side, OrderStatus status, int qty, double price)
    {
        EXPECT_EQ(std::string_view(e.orderId), orderId);
        EXPECT_EQ(std::string_view(e.clientOrderId), clientId);
        EXPECT_EQ(e.side, side);
        EXPECT_EQ(e.status, status);
        EXPECT_EQ(e.quantity, qty);
        EXPECT_DOUBLE_EQ(e.price, price);
    }
}

TEST(OrderBookTest, SingleSellOrderRestsAsNew)
{
    OrderBook book;
    Reports reports;
    auto ts = test_helpers::fixedTimestamp();

    auto o = makeOrder("aa13", "ord1", Instrument::Rose, Side::Sell, 55.0, 100);
    book.processOrder(o, reports, ts);

    ASSERT_EQ(reports.size(), 1u);
    expectExec(execAt(reports, 0), "ord1", "aa13", Side::Sell, OrderStatus::New, 100, 55.0);
}

TEST(OrderBookTest, NoMatchWhenBuyPriceBelowAllAsks)
{
    OrderBook book;
    Reports reports;
    auto ts = test_helpers::fixedTimestamp();

    auto s1 = makeOrder("aa13", "ord1", Instrument::Rose, Side::Sell, 55.0, 100);
    auto s2 = makeOrder("aa14", "ord2", Instrument::Rose, Side::Sell, 45.0, 100);
    auto b1 = makeOrder("aa15", "ord3", Instrument::Rose, Side::Buy, 35.0, 100);

    book.processOrder(s1, reports, ts);
    book.processOrder(s2, reports, ts);
    book.processOrder(b1, reports, ts);

    ASSERT_EQ(reports.size(), 3u);
    expectExec(execAt(reports, 0), "ord1", "aa13", Side::Sell, OrderStatus::New, 100, 55.0);
    expectExec(execAt(reports, 1), "ord2", "aa14", Side::Sell, OrderStatus::New, 100, 45.0);
    expectExec(execAt(reports, 2), "ord3", "aa15", Side::Buy, OrderStatus::New, 100, 35.0);
}

TEST(OrderBookTest, ExactMatchBothFill)
{
    OrderBook book;
    Reports reports;
    auto ts = test_helpers::fixedTimestamp();

    auto s1 = makeOrder("aa13", "ord1", Instrument::Rose, Side::Sell, 55.0, 100);
    auto s2 = makeOrder("aa14", "ord2", Instrument::Rose, Side::Sell, 45.0, 100);
    auto b1 = makeOrder("aa15", "ord3", Instrument::Rose, Side::Buy, 45.0, 100);

    book.processOrder(s1, reports, ts);
    book.processOrder(s2, reports, ts);
    book.processOrder(b1, reports, ts);

    ASSERT_EQ(reports.size(), 4u);

    expectExec(execAt(reports, 2), "ord3", "aa15", Side::Buy, OrderStatus::Fill, 100, 45.0);

    expectExec(execAt(reports, 3), "ord2", "aa14", Side::Sell, OrderStatus::Fill, 100, 45.0);
}

TEST(OrderBookTest, PartialFillIncomingLargerThanResting)
{
    OrderBook book;
    Reports reports;
    auto ts = test_helpers::fixedTimestamp();

    auto s1 = makeOrder("aa13", "ord1", Instrument::Rose, Side::Sell, 55.0, 100);
    auto s2 = makeOrder("aa14", "ord2", Instrument::Rose, Side::Sell, 45.0, 100);
    auto b1 = makeOrder("aa15", "ord3", Instrument::Rose, Side::Buy, 45.0, 200);

    book.processOrder(s1, reports, ts);
    book.processOrder(s2, reports, ts);
    book.processOrder(b1, reports, ts);

    ASSERT_EQ(reports.size(), 4u);

    expectExec(execAt(reports, 2), "ord3", "aa15", Side::Buy, OrderStatus::PFill, 100, 45.0);

    expectExec(execAt(reports, 3), "ord2", "aa14", Side::Sell, OrderStatus::Fill, 100, 45.0);
}

TEST(OrderBookTest, SellSweepsMultipleBuyLevels)
{
    OrderBook book;
    Reports reports;
    auto ts = test_helpers::fixedTimestamp();

    auto b1 = makeOrder("aa13", "ord1", Instrument::Rose, Side::Buy, 55.0, 100);
    auto b2 = makeOrder("aa14", "ord2", Instrument::Rose, Side::Buy, 65.0, 100);

    auto s1 = makeOrder("aa15", "ord3", Instrument::Rose, Side::Sell, 1.0, 300);

    book.processOrder(b1, reports, ts);
    book.processOrder(b2, reports, ts);
    book.processOrder(s1, reports, ts);

    ASSERT_EQ(reports.size(), 6u);

    expectExec(execAt(reports, 2), "ord3", "aa15", Side::Sell, OrderStatus::PFill, 100, 65.0);
    expectExec(execAt(reports, 3), "ord2", "aa14", Side::Buy, OrderStatus::Fill, 100, 65.0);

    expectExec(execAt(reports, 4), "ord3", "aa15", Side::Sell, OrderStatus::PFill, 100, 55.0);
    expectExec(execAt(reports, 5), "ord1", "aa13", Side::Buy, OrderStatus::Fill, 100, 55.0);
}

TEST(OrderBookTest, SellSweepsThenNewBuyMatchesRemainder)
{
    OrderBook book;
    Reports reports;
    auto ts = test_helpers::fixedTimestamp();

    auto b1 = makeOrder("aa13", "ord1", Instrument::Rose, Side::Buy, 55.0, 100);
    auto b2 = makeOrder("aa14", "ord2", Instrument::Rose, Side::Buy, 65.0, 100);
    auto s1 = makeOrder("aa15", "ord3", Instrument::Rose, Side::Sell, 1.0, 300);
    auto b3 = makeOrder("aa16", "ord4", Instrument::Rose, Side::Buy, 2.0, 100);

    book.processOrder(b1, reports, ts);
    book.processOrder(b2, reports, ts);
    book.processOrder(s1, reports, ts);
    book.processOrder(b3, reports, ts);

    ASSERT_EQ(reports.size(), 8u);

    expectExec(execAt(reports, 6), "ord4", "aa16", Side::Buy, OrderStatus::Fill, 100, 1.0);
    expectExec(execAt(reports, 7), "ord3", "aa15", Side::Sell, OrderStatus::Fill, 100, 1.0);
}

TEST(OrderBookTest, SingleBuyOrderRestsAsNew)
{
    OrderBook book;
    Reports reports;
    auto ts = test_helpers::fixedTimestamp();

    auto o = makeOrder("c1", "o1", Instrument::Lavender, Side::Buy, 100.0, 200);
    book.processOrder(o, reports, ts);

    ASSERT_EQ(reports.size(), 1u);
    expectExec(execAt(reports, 0), "o1", "c1", Side::Buy, OrderStatus::New, 200, 100.0);
}

TEST(OrderBookTest, PartialFillRestingSideHasLargerQty)
{

    OrderBook book;
    Reports reports;
    auto ts = test_helpers::fixedTimestamp();

    auto s = makeOrder("s1", "os1", Instrument::Rose, Side::Sell, 50.0, 200);
    auto b = makeOrder("b1", "ob1", Instrument::Rose, Side::Buy, 50.0, 100);

    book.processOrder(s, reports, ts);
    book.processOrder(b, reports, ts);

    ASSERT_EQ(reports.size(), 3u);

    expectExec(execAt(reports, 1), "ob1", "b1", Side::Buy, OrderStatus::Fill, 100, 50.0);

    expectExec(execAt(reports, 2), "os1", "s1", Side::Sell, OrderStatus::PFill, 100, 50.0);
}

TEST(OrderBookTest, MatchOccursAtPassivePrice)
{

    OrderBook book;
    Reports reports;
    auto ts = test_helpers::fixedTimestamp();

    auto s = makeOrder("s1", "os1", Instrument::Rose, Side::Sell, 40.0, 100);
    auto b = makeOrder("b1", "ob1", Instrument::Rose, Side::Buy, 50.0, 100);

    book.processOrder(s, reports, ts);
    book.processOrder(b, reports, ts);

    ASSERT_EQ(reports.size(), 3u);

    expectExec(execAt(reports, 1), "ob1", "b1", Side::Buy, OrderStatus::Fill, 100, 40.0);
    expectExec(execAt(reports, 2), "os1", "s1", Side::Sell, OrderStatus::Fill, 100, 40.0);
}

TEST(OrderBookTest, TimestampPropagatedToReports)
{
    OrderBook book;
    Reports reports;
    auto ts = test_helpers::makeTimestamp("20260315-093000.123");

    auto o = makeOrder("c1", "o1", Instrument::Rose, Side::Buy, 10.0, 100);
    book.processOrder(o, reports, ts);

    ASSERT_EQ(reports.size(), 1u);
    EXPECT_EQ(std::string_view(execAt(reports, 0).transactionTime), "20260315-093000.123");
}
