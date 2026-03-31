#include <gtest/gtest.h>
#include "Utils/FixedString.hpp"
#include <string_view>

TEST(FixedStringTest, DefaultConstructionIsEmpty)
{
    FixedString<16> fs;
    EXPECT_TRUE(fs.empty());
    EXPECT_EQ(fs.size(), 0u);
    EXPECT_EQ(std::string_view(fs), "");
}

TEST(FixedStringTest, ConstructFromStringView)
{
    FixedString<16> fs(std::string_view("hello"));
    EXPECT_EQ(fs.size(), 5u);
    EXPECT_FALSE(fs.empty());
    EXPECT_EQ(std::string_view(fs), "hello");
}

TEST(FixedStringTest, ConstructFromCString)
{
    FixedString<16> fs("world");
    EXPECT_EQ(std::string_view(fs), "world");
}

TEST(FixedStringTest, TruncatesWhenInputExceedsCapacity)
{

    FixedString<8> fs("abcdefghij");
    EXPECT_EQ(fs.size(), 7u);
    EXPECT_EQ(std::string_view(fs), "abcdefg");
}

TEST(FixedStringTest, ExactCapacityMinusOne)
{
    FixedString<8> fs("abcdefg");
    EXPECT_EQ(fs.size(), 7u);
    EXPECT_EQ(std::string_view(fs), "abcdefg");
}

TEST(FixedStringTest, ImplicitConversionToStringView)
{
    FixedString<16> fs("test123");
    std::string_view sv = fs;
    EXPECT_EQ(sv, "test123");
}

TEST(FixedStringTest, DataPointerReturnsNullTerminatedLikeBuffer)
{
    FixedString<16> fs("abc");
    EXPECT_EQ(fs.data()[0], 'a');
    EXPECT_EQ(fs.data()[1], 'b');
    EXPECT_EQ(fs.data()[2], 'c');
}
