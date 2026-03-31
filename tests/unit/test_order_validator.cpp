#include <gtest/gtest.h>
#include "Core/OrderValidator.hpp"
#include "IO/CsvReader.hpp"
#include <variant>
#include <string>

namespace
{

    CsvRow makeRow(std::string_view f0 = {},
                   std::string_view f1 = {},
                   std::string_view f2 = {},
                   std::string_view f3 = {},
                   std::string_view f4 = {},
                   std::size_t fieldCount = 5)
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

    bool isValid(const std::variant<ValidationResult, std::string> &v)
    {
        return std::holds_alternative<ValidationResult>(v);
    }

    std::string rejectReason(const std::variant<ValidationResult, std::string> &v)
    {
        return std::get<std::string>(v);
    }
}

TEST(OrderValidatorTest, AcceptValidBuyRose)
{
    auto result = OrderValidator::validate(makeRow("cli1", "Rose", "1", "100", "55"));
    ASSERT_TRUE(isValid(result));
    auto &vr = std::get<ValidationResult>(result);
    EXPECT_EQ(vr.instrument, Instrument::Rose);
    EXPECT_EQ(vr.side, Side::Buy);
    EXPECT_EQ(vr.quantity, 100);
    EXPECT_DOUBLE_EQ(vr.price, 55.0);
}

TEST(OrderValidatorTest, AcceptValidSellLavender)
{
    auto result = OrderValidator::validate(makeRow("cli2", "Lavender", "2", "500", "12.5"));
    ASSERT_TRUE(isValid(result));
    auto &vr = std::get<ValidationResult>(result);
    EXPECT_EQ(vr.instrument, Instrument::Lavender);
    EXPECT_EQ(vr.side, Side::Sell);
    EXPECT_EQ(vr.quantity, 500);
    EXPECT_DOUBLE_EQ(vr.price, 12.5);
}

TEST(OrderValidatorTest, AcceptAllInstruments)
{
    for (auto &[name, expected] : std::vector<std::pair<const char *, Instrument>>{
             {"Rose", Instrument::Rose},
             {"Lavender", Instrument::Lavender},
             {"Lotus", Instrument::Lotus},
             {"Tulip", Instrument::Tulip},
             {"Orchid", Instrument::Orchid}})
    {
        auto result = OrderValidator::validate(makeRow("c", name, "1", "100", "10"));
        ASSERT_TRUE(isValid(result)) << "Failed for instrument: " << name;
        EXPECT_EQ(std::get<ValidationResult>(result).instrument, expected);
    }
}

TEST(OrderValidatorTest, AcceptBoundaryQuantities)
{

    auto r10 = OrderValidator::validate(makeRow("c", "Rose", "1", "10", "1"));
    ASSERT_TRUE(isValid(r10));
    EXPECT_EQ(std::get<ValidationResult>(r10).quantity, 10);

    auto r1000 = OrderValidator::validate(makeRow("c", "Rose", "1", "1000", "1"));
    ASSERT_TRUE(isValid(r1000));
    EXPECT_EQ(std::get<ValidationResult>(r1000).quantity, 1000);
}

TEST(OrderValidatorTest, RejectEmptyInstrument)
{
    auto result = OrderValidator::validate(makeRow("aa13", "", "1", "100", "55"));
    ASSERT_FALSE(isValid(result));
    EXPECT_EQ(rejectReason(result), "Invalid instrument");
}

TEST(OrderValidatorTest, RejectUnknownInstrument)
{
    auto result = OrderValidator::validate(makeRow("c", "Daisy", "1", "100", "10"));
    ASSERT_FALSE(isValid(result));
    EXPECT_EQ(rejectReason(result), "Invalid instrument");
}

TEST(OrderValidatorTest, RejectInvalidSide)
{
    auto result = OrderValidator::validate(makeRow("aa14", "Rose", "3", "100", "65"));
    ASSERT_FALSE(isValid(result));
    EXPECT_EQ(rejectReason(result), "Invalid side");
}

TEST(OrderValidatorTest, RejectEmptySide)
{
    auto result = OrderValidator::validate(makeRow("c", "Rose", "", "100", "10"));
    ASSERT_FALSE(isValid(result));
    EXPECT_EQ(rejectReason(result), "Invalid side");
}

TEST(OrderValidatorTest, RejectQuantityNotMultipleOfTen)
{
    auto result = OrderValidator::validate(makeRow("aa15", "Lavender", "2", "101", "1"));
    ASSERT_FALSE(isValid(result));
    EXPECT_EQ(rejectReason(result), "Invalid size");
}

TEST(OrderValidatorTest, RejectQuantityBelowMinimum)
{
    auto result = OrderValidator::validate(makeRow("c", "Rose", "1", "0", "10"));
    ASSERT_FALSE(isValid(result));
    EXPECT_EQ(rejectReason(result), "Invalid size");
}

TEST(OrderValidatorTest, RejectQuantityAboveMaximum)
{
    auto result = OrderValidator::validate(makeRow("c", "Rose", "1", "1010", "10"));
    ASSERT_FALSE(isValid(result));
    EXPECT_EQ(rejectReason(result), "Invalid size");
}

TEST(OrderValidatorTest, RejectEmptyQuantity)
{
    auto result = OrderValidator::validate(makeRow("c", "Rose", "1", "", "10"));
    ASSERT_FALSE(isValid(result));
    EXPECT_EQ(rejectReason(result), "Invalid size");
}

TEST(OrderValidatorTest, RejectNegativePrice)
{
    auto result = OrderValidator::validate(makeRow("aa16", "Tulip", "1", "100", "-1"));
    ASSERT_FALSE(isValid(result));
    EXPECT_EQ(rejectReason(result), "Invalid price");
}

TEST(OrderValidatorTest, RejectZeroPrice)
{
    auto result = OrderValidator::validate(makeRow("c", "Rose", "1", "100", "0"));
    ASSERT_FALSE(isValid(result));
    EXPECT_EQ(rejectReason(result), "Invalid price");
}

TEST(OrderValidatorTest, RejectEmptyPrice)
{
    auto result = OrderValidator::validate(makeRow("c", "Rose", "1", "100", ""));
    ASSERT_FALSE(isValid(result));
    EXPECT_EQ(rejectReason(result), "Invalid price");
}

TEST(OrderValidatorTest, RejectMissingFields)
{
    auto result = OrderValidator::validate(makeRow("aa13", "Rose", "1", "100", "", 4));
    ASSERT_FALSE(isValid(result));
    EXPECT_EQ(rejectReason(result), "Missing fields");
}

TEST(OrderValidatorTest, PriceValidatedBeforeQuantityWhenBothInvalid)
{

    auto result = OrderValidator::validate(makeRow("c", "Orchid", "1", "1001", "-1"));
    ASSERT_FALSE(isValid(result));
    EXPECT_EQ(rejectReason(result), "Invalid price");
}
