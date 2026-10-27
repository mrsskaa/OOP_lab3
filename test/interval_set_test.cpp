#include "lab03/interval_set.hpp"

#include <sstream>
#include <type_traits>
#include <utility>

#include <gtest/gtest.h>

namespace lab03 {
namespace {

TEST(IntervalSetTest, DefaultIsEmpty) {
    const IntervalSet set;
    EXPECT_EQ(set.size(), 0U);
    EXPECT_TRUE(set.empty());
    EXPECT_FALSE(set.contains(Interval(0, 1)));
    EXPECT_FALSE(set(Interval(0, 1)));
}

TEST(IntervalSetTest, AddsAndSortsDisjointIntervals) {
    IntervalSet set;
    EXPECT_TRUE(set.add(Interval(5, 6)));
    EXPECT_TRUE(set.add(Interval(-3, -1)));
    EXPECT_TRUE(set.add(Interval(1, 2)));
    EXPECT_EQ(set.size(), 3U);
    std::ostringstream out;
    out << set;
    EXPECT_EQ(out.str(), "{ [-3, -1], [1, 2], [5, 6] }");
}

TEST(IntervalSetTest_Fail, DuplicateAndContainedIntervalDoNotChangeSet) {
    IntervalSet set;
    set += Interval(0, 5);
    EXPECT_FALSE(set.add(Interval(0, 5)));
    EXPECT_FALSE(set.add(Interval(1, 2)));
    EXPECT_EQ(set.size(), 1U);
    EXPECT_TRUE(set.contains(Interval(0, 5)));
}

TEST(IntervalSetTest, MergesOverlapAndTouch) {
    IntervalSet set;
    set += Interval(1, 3);
    set += Interval(5, 7);
    EXPECT_TRUE(set.add(Interval(3, 5)));
    EXPECT_EQ(set.size(), 1U);
    EXPECT_TRUE(set.contains(Interval(1, 7)));
}

TEST(IntervalSetTest, NewIntervalCanCoverManyElements) {
    IntervalSet set;
    set += Interval(1, 2);
    set += Interval(4, 5);
    set += Interval(8, 9);
    set += Interval(-1, 12);
    EXPECT_EQ(set.size(), 1U);
    EXPECT_TRUE(set.contains(Interval(-1, 12)));
}

TEST(IntervalSetTest, OpenGapIsPreservedUntilPointIsAdded) {
    IntervalSet set;
    set += Interval(0, 1, true, false);
    set += Interval(1, 2, false, true);
    EXPECT_EQ(set.size(), 2U);
    set += Interval(1, 1);
    EXPECT_EQ(set.size(), 1U);
    EXPECT_TRUE(set.contains(Interval(0, 2)));
}

TEST(IntervalSetTest, ContainsAndCallSearchExactCanonicalElement) {
    IntervalSet set;
    set += Interval(1, 5);
    EXPECT_TRUE(set.contains(Interval(1, 5)));
    EXPECT_TRUE(set(Interval(1, 5)));
    EXPECT_FALSE(set.contains(Interval(2, 3)));
    EXPECT_FALSE(set(Interval(2, 3)));
}

TEST(IntervalSetTest, RemovesFirstMiddleAndLast) {
    IntervalSet set;
    set += Interval(0, 1);
    set += Interval(3, 4);
    set += Interval(6, 7);
    EXPECT_TRUE(set.remove(Interval(3, 4)));
    EXPECT_TRUE(set.remove(Interval(0, 1)));
    EXPECT_TRUE(set.remove(Interval(6, 7)));
    EXPECT_TRUE(set.empty());
    EXPECT_TRUE(set.add(Interval(10, 11)));
}

TEST(IntervalSetTest_Fail, MissingAndPartialRemovalDoNotChangeSet) {
    IntervalSet set;
    EXPECT_FALSE(set.remove(Interval(0, 1)));
    set += Interval(0, 5);
    EXPECT_FALSE(set.remove(Interval(1, 2)));
    EXPECT_EQ(set.size(), 1U);
}

TEST(IntervalSetTest, CompoundOperatorsReturnSelf) {
    IntervalSet set;
    EXPECT_EQ(&(set += Interval(0, 1)), &set);
    EXPECT_EQ(&(set -= Interval(0, 1)), &set);
    EXPECT_TRUE(set.empty());
}

TEST(IntervalSetTest, CopyConstructorCreatesIndependentStorage) {
    IntervalSet original;
    original += Interval(1, 3);
    IntervalSet copy(original);
    copy += Interval(3, 5);
    EXPECT_TRUE(original.contains(Interval(1, 3)));
    EXPECT_TRUE(copy.contains(Interval(1, 5)));
}

TEST(IntervalSetTest, CopyAssignmentReplacesContentsAndIsIndependent) {
    IntervalSet original;
    original += Interval(1, 3);
    IntervalSet copy;
    copy += Interval(10, 20);
    EXPECT_EQ(&(copy = original), &copy);
    original -= Interval(1, 3);
    EXPECT_TRUE(copy.contains(Interval(1, 3)));
    EXPECT_EQ(copy.size(), 1U);
    copy = original;
    EXPECT_TRUE(copy.empty());
}

TEST(IntervalSetTest, SelfCopyKeepsContents) {
    IntervalSet set;
    set += Interval(1, 3);
    const IntervalSet& alias = set;
    set = alias;
    EXPECT_TRUE(set.contains(Interval(1, 3)));
}

TEST(IntervalSetTest, MoveConstructorEmptiesReusableSource) {
    IntervalSet source;
    source += Interval(1, 3);
    IntervalSet target(std::move(source));
    EXPECT_TRUE(target.contains(Interval(1, 3)));
    EXPECT_TRUE(source.empty());
    EXPECT_TRUE(source.add(Interval(5, 6)));
    EXPECT_EQ(target.size(), 1U);
}

TEST(IntervalSetTest, MoveAssignmentReplacesResourceAndEmptiesSource) {
    IntervalSet source;
    source += Interval(1, 3);
    IntervalSet target;
    target += Interval(5, 6);
    EXPECT_EQ(&(target = std::move(source)), &target);
    EXPECT_TRUE(source.empty());
    EXPECT_TRUE(target.contains(Interval(1, 3)));
    EXPECT_EQ(target.size(), 1U);
    EXPECT_TRUE(source.add(Interval(9, 10)));
}

TEST(IntervalSetTest, SelfMoveKeepsContents) {
    IntervalSet set;
    set += Interval(1, 3);
    IntervalSet& alias = set;
    set = std::move(alias);
    EXPECT_TRUE(set.contains(Interval(1, 3)));
}

TEST(IntervalSetTest, CopiesAndMovesEmptyObjects) {
    IntervalSet empty;
    IntervalSet copy(empty);
    IntervalSet moved(std::move(empty));
    copy = moved;
    moved = std::move(copy);
    EXPECT_TRUE(empty.empty());
    EXPECT_TRUE(copy.empty());
    EXPECT_TRUE(moved.empty());
    EXPECT_TRUE(moved.add(Interval(1, 1)));
}

TEST(IntervalSetTest, MovesAndSwapAreNoexcept) {
    static_assert(std::is_nothrow_move_constructible_v<IntervalSet>);
    static_assert(std::is_nothrow_move_assignable_v<IntervalSet>);
    IntervalSet first;
    IntervalSet second;
    first += Interval(1, 2);
    second += Interval(4, 5);
    first.swap(second);
    EXPECT_TRUE(first.contains(Interval(4, 5)));
    EXPECT_TRUE(second.contains(Interval(1, 2)));
}

TEST(IntervalSetTest, EqualityUsesCanonicalCoverage) {
    IntervalSet first;
    first += Interval(0, 2);
    first += Interval(2, 4);
    IntervalSet second;
    second += Interval(0, 4);
    EXPECT_TRUE(first == second);
    EXPECT_FALSE(first != second);
    second += Interval(8, 9);
    EXPECT_TRUE(first != second);
    EXPECT_FALSE(first == second);
}

TEST(IntervalSetTest, UnionMergesBothOperandsWithoutChangingThem) {
    IntervalSet first;
    first += Interval(0, 2);
    first += Interval(8, 10);
    IntervalSet second;
    second += Interval(2, 9);
    const IntervalSet result = first | second;
    EXPECT_EQ(result.size(), 1U);
    EXPECT_TRUE(result.contains(Interval(0, 10)));
    EXPECT_EQ(first.size(), 2U);
    EXPECT_TRUE(second.contains(Interval(2, 9)));
}

TEST(IntervalSetTest, IntersectionFindsMultiplePieces) {
    IntervalSet first;
    first += Interval(0, 3);
    first += Interval(5, 8);
    IntervalSet second;
    second += Interval(2, 6);
    const IntervalSet result = first & second;
    EXPECT_EQ(result.size(), 2U);
    EXPECT_TRUE(result.contains(Interval(2, 3)));
    EXPECT_TRUE(result.contains(Interval(5, 6)));
}

TEST(IntervalSetTest, ClosedTouchIntersectsInOnePoint) {
    IntervalSet first;
    first += Interval(0, 2);
    IntervalSet second;
    second += Interval(2, 4);
    EXPECT_TRUE((first & second).contains(Interval(2, 2)));
}

TEST(IntervalSetTest, OpenTouchHasEmptyIntersection) {
    IntervalSet first;
    first += Interval(0, 2, true, false);
    IntervalSet second;
    second += Interval(2, 4);
    EXPECT_TRUE((first & second).empty());
}

TEST(IntervalSetTest, DifferenceSplitsAndExcludesCutEndpoints) {
    IntervalSet first;
    first += Interval(1, 5);
    IntervalSet second;
    second += Interval(2, 3);
    const IntervalSet result = first - second;
    EXPECT_EQ(result.size(), 2U);
    EXPECT_TRUE(result.contains(Interval(1, 2, true, false)));
    EXPECT_TRUE(result.contains(Interval(3, 5, false, true)));
    EXPECT_TRUE(first.contains(Interval(1, 5)));
    EXPECT_TRUE(second.contains(Interval(2, 3)));
}

TEST(IntervalSetTest, DifferenceRemovesMultiplePiecesAndPoint) {
    IntervalSet first;
    first += Interval(0, 10);
    IntervalSet second;
    second += Interval(-1, 1);
    second += Interval(3, 4);
    second += Interval(6, 6);
    second += Interval(9, 12);
    const IntervalSet result = first - second;
    EXPECT_EQ(result.size(), 3U);
    EXPECT_TRUE(result.contains(Interval(1, 3, false, false)));
    EXPECT_TRUE(result.contains(Interval(4, 6, false, false)));
    EXPECT_TRUE(result.contains(Interval(6, 9, false, false)));
}

TEST(IntervalSetTest, DifferenceCanKeepOnlyEndpoints) {
    IntervalSet first;
    first += Interval(1, 3);
    IntervalSet second;
    second += Interval(1, 3, false, false);
    const IntervalSet result = first - second;
    EXPECT_EQ(result.size(), 2U);
    EXPECT_TRUE(result.contains(Interval(1, 1)));
    EXPECT_TRUE(result.contains(Interval(3, 3)));
}

TEST(IntervalSetTest, DifferenceHandlesTouchAndCover) {
    IntervalSet first;
    first += Interval(1, 3);
    IntervalSet second;
    second += Interval(3, 5);
    EXPECT_TRUE((first - second).contains(Interval(1, 3, true, false)));
    second += Interval(-1, 3);
    EXPECT_TRUE((first - second).empty());
}

TEST(IntervalSetTest, EmptyAndSelfOperations) {
    const IntervalSet empty;
    IntervalSet set;
    set += Interval(1, 4);
    EXPECT_TRUE((set | empty) == set);
    EXPECT_TRUE((empty | set) == set);
    EXPECT_TRUE((set & empty).empty());
    EXPECT_TRUE((empty & set).empty());
    EXPECT_TRUE((set - empty) == set);
    EXPECT_TRUE((empty - set).empty());
    EXPECT_TRUE((set | set) == set);
    EXPECT_TRUE((set & set) == set);
    EXPECT_TRUE((set - set).empty());
}

TEST(IntervalSetTest, OutputIncludesSortedIntervalsAndEmptySet) {
    std::ostringstream out;
    const IntervalSet empty;
    IntervalSet set;
    set += Interval(4, 5, false, true);
    set += Interval(-1, 2);
    out << empty << ' ' << set;
    EXPECT_EQ(out.str(), "{} { [-1, 2], (4, 5] }");
}

TEST(IntervalSetTest, ThousandInsertionsGrowthAndDuplicateCheck) {
    IntervalSet set;
    for (int i = 999; i >= 0; --i) {
        ASSERT_TRUE(set.add(Interval(i * 3, i * 3 + 1)));
    }
    ASSERT_EQ(set.size(), 1000U);
    for (int i = 0; i < 1000; ++i) {
        EXPECT_TRUE(set.contains(Interval(i * 3, i * 3 + 1)));
        EXPECT_FALSE(set.add(Interval(i * 3, i * 3 + 1)));
    }
    EXPECT_EQ(set.size(), 1000U);
}

TEST(IntervalSetTest, ThousandTouchingIntervalsCollapseIntoOne) {
    IntervalSet set;
    for (int i = 0; i < 1000; ++i) {
        set += Interval(i, i + 1);
    }
    EXPECT_EQ(set.size(), 1U);
    EXPECT_TRUE(set.contains(Interval(0, 1000)));
}

}  // namespace
}  // namespace lab03
