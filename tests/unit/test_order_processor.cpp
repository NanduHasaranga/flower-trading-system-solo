#include <gtest/gtest.h>
#include "IO/OrderProcessor.hpp"
#include "IO/CsvReader.hpp"
#include "Core/Order.hpp"
#include "Core/OrderReject.hpp"
#include "test_helpers.hpp"
#include <variant>
#include <string_view>

namespace
{
    CsvRow makeRow(std::string_view f0, std::string_view f1,
                   std::string_view f2, std::string_view f3,
                   std::string_view f4, std::size_t fieldCount = 5)
    {
        CsvRow row;
        row.fields[0] = f0;
        row.fields[1] = f1;
        row.fields[2] = f2;
        row.fields[3] = f3;
        row.fields[4] = f4;
        row.fieldCount = fieldCount;
        return row;
    }
}

class OrderProcessorTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        test_helpers::resetIds();
    }

    utils::Timestamp ts = test_helpers::fixedTimestamp();
};

TEST_F(OrderProcessorTest, ValidOrderBuildsOrderWithCorrectFields)
{
    auto row = makeRow("aa13", "Rose", "2", "100", "55");
    auto result = OrderProcessor::processRow(row, ts);

    ASSERT_TRUE(std::holds_alternative<Order>(result));
    const auto &order = std::get<Order>(result);

    EXPECT_EQ(std::string_view(order.clientOrderId), "aa13");
    EXPECT_EQ(std::string_view(order.orderId), "ord1");
    EXPECT_EQ(order.instrument, Instrument::Rose);
    EXPECT_EQ(order.side, Side::Sell);
    EXPECT_EQ(order.quantity, 100);
    EXPECT_DOUBLE_EQ(order.price, 55.0);
}

TEST_F(OrderProcessorTest, SequentialOrdersGetIncrementingIds)
{
    auto row1 = makeRow("c1", "Rose", "1", "100", "10");
    auto row2 = makeRow("c2", "Tulip", "2", "200", "20");

    auto r1 = OrderProcessor::processRow(row1, ts);
    auto r2 = OrderProcessor::processRow(row2, ts);

    ASSERT_TRUE(std::holds_alternative<Order>(r1));
    ASSERT_TRUE(std::holds_alternative<Order>(r2));

    EXPECT_EQ(std::string_view(std::get<Order>(r1).orderId), "ord1");
    EXPECT_EQ(std::string_view(std::get<Order>(r2).orderId), "ord2");
}

TEST_F(OrderProcessorTest, InvalidInstrumentProducesReject)
{
    auto row = makeRow("aa13", "", "1", "100", "55");
    auto result = OrderProcessor::processRow(row, ts);

    ASSERT_TRUE(std::holds_alternative<OrderReject>(result));
    const auto &reject = std::get<OrderReject>(result);

    EXPECT_EQ(reject.clientOrderId, "aa13");
    EXPECT_EQ(reject.orderId, "ord1");
    EXPECT_EQ(reject.reason, "Invalid instrument");
    EXPECT_EQ(std::string_view(reject.timestamp), std::string_view(ts));
}

TEST_F(OrderProcessorTest, InvalidSideProducesReject)
{
    auto row = makeRow("aa14", "Rose", "3", "100", "65");
    auto result = OrderProcessor::processRow(row, ts);

    ASSERT_TRUE(std::holds_alternative<OrderReject>(result));
    EXPECT_EQ(std::get<OrderReject>(result).reason, "Invalid side");
}

TEST_F(OrderProcessorTest, InvalidQuantityProducesReject)
{
    auto row = makeRow("aa15", "Lavender", "2", "101", "1");
    auto result = OrderProcessor::processRow(row, ts);

    ASSERT_TRUE(std::holds_alternative<OrderReject>(result));
    EXPECT_EQ(std::get<OrderReject>(result).reason, "Invalid size");
}

TEST_F(OrderProcessorTest, InvalidPriceProducesReject)
{
    auto row = makeRow("aa16", "Tulip", "1", "100", "-1");
    auto result = OrderProcessor::processRow(row, ts);

    ASSERT_TRUE(std::holds_alternative<OrderReject>(result));
    EXPECT_EQ(std::get<OrderReject>(result).reason, "Invalid price");
}

TEST_F(OrderProcessorTest, MissingFieldsProducesReject)
{
    auto row = makeRow("aa13", "Rose", "1", "100", "", 4);
    auto result = OrderProcessor::processRow(row, ts);

    ASSERT_TRUE(std::holds_alternative<OrderReject>(result));
    EXPECT_EQ(std::get<OrderReject>(result).reason, "Missing fields");
}

TEST_F(OrderProcessorTest, RejectCarriesRawFieldValues)
{
    auto row = makeRow("cli99", "BadFlower", "1", "200", "50");
    auto result = OrderProcessor::processRow(row, ts);

    ASSERT_TRUE(std::holds_alternative<OrderReject>(result));
    const auto &reject = std::get<OrderReject>(result);

    EXPECT_EQ(reject.clientOrderId, "cli99");
    EXPECT_EQ(reject.instrument, "BadFlower");
    EXPECT_EQ(reject.side, "1");
    EXPECT_EQ(reject.quantity, "200");
    EXPECT_EQ(reject.price, "50");
}

TEST_F(OrderProcessorTest, RejectUsesInjectedTimestamp)
{
    auto customTs = test_helpers::makeTimestamp("20260601-150000.999");
    auto row = makeRow("c1", "", "1", "100", "10");
    auto result = OrderProcessor::processRow(row, customTs);

    ASSERT_TRUE(std::holds_alternative<OrderReject>(result));
    EXPECT_EQ(std::string_view(std::get<OrderReject>(result).timestamp),
              "20260601-150000.999");
}

TEST_F(OrderProcessorTest, ValidAndRejectOrdersShareIdSequence)
{
    auto valid = makeRow("c1", "Rose", "1", "100", "10");
    auto invalid = makeRow("c2", "", "1", "100", "10");

    auto r1 = OrderProcessor::processRow(valid, ts);
    auto r2 = OrderProcessor::processRow(invalid, ts);

    EXPECT_EQ(std::string_view(std::get<Order>(r1).orderId), "ord1");
    EXPECT_EQ(std::get<OrderReject>(r2).orderId, "ord2");
}
