#include "lab03/interval.hpp"

#include <sstream>

#include <gtest/gtest.h>

namespace lab03 {
namespace {

TEST(IntervalTest, DefaultIsZeroPoint) {
    const Interval interval;
    EXPECT_EQ(interval.left(), 0);
    EXPECT_EQ(interval.right(), 0);
    EXPECT_TRUE(interval.left_closed());
    EXPECT_TRUE(interval.right_closed());
}

TEST(IntervalTest, StoresBoundsAndEndpointFlags) {
    const Interval interval(-2.5, 4, false, true);
    EXPECT_EQ(interval.left(), -2.5);
    EXPECT_EQ(interval.right(), 4);
    EXPECT_FALSE(interval.left_closed());
    EXPECT_TRUE(interval.right_closed());
}

TEST(IntervalTest, EqualityIncludesEndpoints) {
    EXPECT_TRUE(Interval(1, 2) == Interval(1, 2));
    EXPECT_FALSE(Interval(1, 2) == Interval(1, 2, false, true));
    EXPECT_FALSE(Interval(1, 2) == Interval(1, 3));
}

TEST(IntervalTest, OrderingIsLexicographic) {
    EXPECT_TRUE(Interval(0, 5) < Interval(1, 2));
    EXPECT_TRUE(Interval(1, 2) < Interval(1, 3));
    EXPECT_TRUE(Interval(1, 2) < Interval(1, 2, false, true));
    EXPECT_FALSE(Interval(1, 2) < Interval(1, 2));
}

TEST(IntervalTest, OutputUsesEndpointBrackets) {
    std::ostringstream out;
    out << Interval(1, 3) << ' ' << Interval(2, 4, false, false);
    EXPECT_EQ(out.str(), "[1, 3] (2, 4)");
}

}  // namespace
}  // namespace lab03
