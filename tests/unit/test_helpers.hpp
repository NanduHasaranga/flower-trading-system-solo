#pragma once

#include "Utils/TimeUtils.hpp"
#include "Utils/OrderIdGenerator.hpp"
#include <cstring>

namespace test_helpers
{
    inline utils::Timestamp makeTimestamp(const char *str)
    {
        utils::Timestamp ts;
        ts.len = static_cast<unsigned char>(std::strlen(str));
        std::memcpy(ts.buf, str, ts.len);
        return ts;
    }

    inline utils::Timestamp fixedTimestamp()
    {
        return makeTimestamp("20260101-120000.000");
    }

    inline void resetIds(long start = 1)
    {
        Utils::OrderIdGenerator::reset(start);
    }

}
