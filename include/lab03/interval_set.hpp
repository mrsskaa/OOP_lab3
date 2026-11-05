#pragma once

#include <cstddef>
#include <iostream>

#include "lab03/interval.hpp"

namespace lab03 {

class IntervalSet {
public:
    IntervalSet() = default;
    IntervalSet(const IntervalSet& other);
    IntervalSet(IntervalSet&& other) noexcept;
    IntervalSet& operator=(const IntervalSet& other);
    IntervalSet& operator=(IntervalSet&& other) noexcept;
    ~IntervalSet();

    bool add(const Interval& interval);
    bool remove(const Interval& interval);
    bool contains(const Interval& interval) const;
    bool operator()(const Interval& interval) const;
    std::size_t size() const;
    bool empty() const;

    IntervalSet& operator+=(const Interval& interval);
    IntervalSet& operator-=(const Interval& interval);
    IntervalSet operator|(const IntervalSet& other) const;
    IntervalSet operator&(const IntervalSet& other) const;
    IntervalSet operator-(const IntervalSet& other) const;
    bool operator==(const IntervalSet& other) const;
    bool operator!=(const IntervalSet& other) const;
    void swap(IntervalSet& other) noexcept;

    friend std::ostream& operator<<(std::ostream& out, const IntervalSet& set);

private:
    void Grow();
    std::size_t Find(const Interval& interval) const;

    Interval* data_ = nullptr;
    std::size_t size_ = 0;
    std::size_t capacity_ = 0;
};

}  // namespace lab03
