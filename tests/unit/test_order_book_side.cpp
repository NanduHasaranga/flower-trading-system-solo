#include <gtest/gtest.h>
#include "Engine/OrderBookSide.hpp"
#include "Core/Order.hpp"

namespace
{
    Order makeOrder(const char *clientId, const char *orderId,
                    Instrument inst, Side side, double price, int qty)
    {
        return Order{
            FixedString<16>(clientId),
            FixedString<16>(orderId),
            inst, side, price, qty};
    }
}

TEST(OrderBookSideTest, BuySideIsEmptyInitially)
{
    OrderBookSide<BuyComparator> buy;
    EXPECT_TRUE(buy.isEmpty());
}

TEST(OrderBookSideTest, BuySideInsertAndGetBest)
{
    OrderBookSide<BuyComparator> buy;
    auto o = makeOrder("c1", "o1", Instrument::Rose, Side::Buy, 50.0, 100);
    buy.insertOrder(std::move(o));

    EXPECT_FALSE(buy.isEmpty());
    EXPECT_DOUBLE_EQ(buy.getBestOrder().price, 50.0);
}

TEST(OrderBookSideTest, BuySidePriorityHighestFirst)
{
    OrderBookSide<BuyComparator> buy;
    auto o1 = makeOrder("c1", "o1", Instrument::Rose, Side::Buy, 45.0, 100);
    auto o2 = makeOrder("c2", "o2", Instrument::Rose, Side::Buy, 55.0, 100);
    auto o3 = makeOrder("c3", "o3", Instrument::Rose, Side::Buy, 50.0, 100);

    buy.insertOrder(std::move(o1));
    buy.insertOrder(std::move(o2));
    buy.insertOrder(std::move(o3));

    EXPECT_DOUBLE_EQ(buy.getBestOrder().price, 55.0);
}

TEST(OrderBookSideTest, BuySideTimePriorityWithinSamePrice)
{
    OrderBookSide<BuyComparator> buy;
    auto o1 = makeOrder("first", "o1", Instrument::Rose, Side::Buy, 50.0, 100);
    auto o2 = makeOrder("second", "o2", Instrument::Rose, Side::Buy, 50.0, 200);

    buy.insertOrder(std::move(o1));
    buy.insertOrder(std::move(o2));

    EXPECT_EQ(std::string_view(buy.getBestOrder().clientOrderId), "first");
}

TEST(OrderBookSideTest, BuySideRemoveTopOrder)
{
    OrderBookSide<BuyComparator> buy;
    auto o1 = makeOrder("c1", "o1", Instrument::Rose, Side::Buy, 55.0, 100);
    auto o2 = makeOrder("c2", "o2", Instrument::Rose, Side::Buy, 45.0, 100);

    buy.insertOrder(std::move(o1));
    buy.insertOrder(std::move(o2));

    buy.removeTopOrder();
    EXPECT_DOUBLE_EQ(buy.getBestOrder().price, 45.0);

    buy.removeTopOrder();
    EXPECT_TRUE(buy.isEmpty());
}

TEST(OrderBookSideTest, BuySideUpdateTopOrderQuantity)
{
    OrderBookSide<BuyComparator> buy;
    auto o = makeOrder("c1", "o1", Instrument::Rose, Side::Buy, 50.0, 300);
    buy.insertOrder(std::move(o));

    buy.updateTopOrderQuantity(100);
    EXPECT_EQ(buy.getBestOrder().quantity, 100);
}

TEST(OrderBookSideTest, SellSideIsEmptyInitially)
{
    OrderBookSide<SellComparator> sell;
    EXPECT_TRUE(sell.isEmpty());
}

TEST(OrderBookSideTest, SellSidePriorityLowestFirst)
{
    OrderBookSide<SellComparator> sell;
    auto o1 = makeOrder("c1", "o1", Instrument::Rose, Side::Sell, 55.0, 100);
    auto o2 = makeOrder("c2", "o2", Instrument::Rose, Side::Sell, 45.0, 100);
    auto o3 = makeOrder("c3", "o3", Instrument::Rose, Side::Sell, 50.0, 100);

    sell.insertOrder(std::move(o1));
    sell.insertOrder(std::move(o2));
    sell.insertOrder(std::move(o3));

    EXPECT_DOUBLE_EQ(sell.getBestOrder().price, 45.0);
}

TEST(OrderBookSideTest, SellSideTimePriorityWithinSamePrice)
{
    OrderBookSide<SellComparator> sell;
    auto o1 = makeOrder("first", "o1", Instrument::Rose, Side::Sell, 50.0, 100);
    auto o2 = makeOrder("second", "o2", Instrument::Rose, Side::Sell, 50.0, 200);

    sell.insertOrder(std::move(o1));
    sell.insertOrder(std::move(o2));

    EXPECT_EQ(std::string_view(sell.getBestOrder().clientOrderId), "first");
}

TEST(OrderBookSideTest, GetBestOnEmptySideThrows)
{
    OrderBookSide<BuyComparator> buy;
    EXPECT_THROW(buy.getBestOrder(), std::runtime_error);
}

TEST(OrderBookSideTest, RemoveOnEmptySideIsNoOp)
{
    OrderBookSide<SellComparator> sell;
    EXPECT_NO_THROW(sell.removeTopOrder());
}
