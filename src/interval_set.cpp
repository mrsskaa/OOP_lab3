#include "lab03/interval_set.hpp"

#include <utility>

namespace lab03 {
namespace {

bool HasPoints(double left, double right, bool left_closed, bool right_closed) {
    return left < right || (left == right && left_closed && right_closed);
}

bool Before(const Interval& first, const Interval& second) {
    return first.right() < second.left() ||
           (first.right() == second.left() && !first.right_closed() && !second.left_closed());
}

Interval Merge(const Interval& first, const Interval& second) {
    double left = first.left();
    bool left_closed = first.left_closed();
    if (second.left() < left) {
        left = second.left();
        left_closed = second.left_closed();
    } else if (second.left() == left) {
        left_closed = left_closed || second.left_closed();
    }
    double right = first.right();
    bool right_closed = first.right_closed();
    if (second.right() > right) {
        right = second.right();
        right_closed = second.right_closed();
    } else if (second.right() == right) {
        right_closed = right_closed || second.right_closed();
    }
    return Interval(left, right, left_closed, right_closed);
}

}  // namespace

IntervalSet::IntervalSet(const IntervalSet& other) : size_(other.size_), capacity_(other.capacity_) {
    if (capacity_ != 0) {
        data_ = new Interval[capacity_];
        for (std::size_t i = 0; i < size_; ++i) {
            data_[i] = other.data_[i];
        }
    }
}

IntervalSet::IntervalSet(IntervalSet&& other) noexcept {
    swap(other);
}

IntervalSet& IntervalSet::operator=(const IntervalSet& other) {
    if (this != &other) {
        IntervalSet copy(other);
        swap(copy);
    }
    return *this;
}

IntervalSet& IntervalSet::operator=(IntervalSet&& other) noexcept {
    if (this != &other) {
        IntervalSet moved(std::move(other));
        swap(moved);
    }
    return *this;
}

IntervalSet::~IntervalSet() {
    delete[] data_;
}

bool IntervalSet::add(const Interval& interval) {
    std::size_t first = 0;
    while (first < size_ && Before(data_[first], interval)) {
        ++first;
    }
    Interval merged = interval;
    std::size_t last = first;
    while (last < size_ && !Before(merged, data_[last])) {
        merged = Merge(merged, data_[last]);
        ++last;
    }
    if (last == first + 1 && data_[first] == merged) {
        return false;
    }
    if (first == last) {
        if (size_ == capacity_) {
            Grow();
        }
        for (std::size_t i = size_; i > first; --i) {
            data_[i] = data_[i - 1];
        }
        ++size_;
    } else {
        for (std::size_t i = last; i < size_; ++i) {
            data_[first + 1 + i - last] = data_[i];
        }
        size_ -= last - first - 1;
    }
    data_[first] = merged;
    return true;
}

bool IntervalSet::remove(const Interval& interval) {
    const std::size_t index = Find(interval);
    if (index == size_) {
        return false;
    }
    for (std::size_t i = index + 1; i < size_; ++i) {
        data_[i - 1] = data_[i];
    }
    --size_;
    return true;
}

bool IntervalSet::contains(const Interval& interval) const {
    return Find(interval) != size_;
}

bool IntervalSet::operator()(const Interval& interval) const {
    return contains(interval);
}

std::size_t IntervalSet::size() const {
    return size_;
}

bool IntervalSet::empty() const {
    return size_ == 0;
}

IntervalSet& IntervalSet::operator+=(const Interval& interval) {
    add(interval);
    return *this;
}

IntervalSet& IntervalSet::operator-=(const Interval& interval) {
    remove(interval);
    return *this;
}

IntervalSet IntervalSet::operator|(const IntervalSet& other) const {
    IntervalSet result(*this);
    for (std::size_t i = 0; i < other.size_; ++i) {
        result.add(other.data_[i]);
    }
    return result;
}

IntervalSet IntervalSet::operator&(const IntervalSet& other) const {
    IntervalSet result;
    std::size_t i = 0;
    std::size_t j = 0;
    while (i < size_ && j < other.size_) {
        const Interval& first = data_[i];
        const Interval& second = other.data_[j];
        const double left = first.left() > second.left() ? first.left() : second.left();
        const double right = first.right() < second.right() ? first.right() : second.right();
        const bool left_closed =
            (left != first.left() || first.left_closed()) && (left != second.left() || second.left_closed());
        const bool right_closed =
            (right != first.right() || first.right_closed()) && (right != second.right() || second.right_closed());
        if (HasPoints(left, right, left_closed, right_closed)) {
            result.add(Interval(left, right, left_closed, right_closed));
        }
        if (first.right() < second.right()) {
            ++i;
        } else if (first.right() > second.right()) {
            ++j;
        } else {
            ++i;
            ++j;
        }
    }
    return result;
}

IntervalSet IntervalSet::operator-(const IntervalSet& other) const {
    IntervalSet result;
    for (std::size_t i = 0; i < size_; ++i) {
        double left = data_[i].left();
        const double right = data_[i].right();
        bool left_closed = data_[i].left_closed();
        const bool right_closed = data_[i].right_closed();
        bool remains = true;
        for (std::size_t j = 0; j < other.size_ && remains; ++j) {
            const Interval& cut = other.data_[j];
            if (cut.right() < left || (cut.right() == left && !(cut.right_closed() && left_closed))) {
                continue;
            }
            if (cut.left() > right || (cut.left() == right && !(cut.left_closed() && right_closed))) {
                break;
            }
            if (HasPoints(left, cut.left(), left_closed, !cut.left_closed())) {
                result.add(Interval(left, cut.left(), left_closed, !cut.left_closed()));
            }
            left = cut.right();
            left_closed = !cut.right_closed();
            remains = HasPoints(left, right, left_closed, right_closed);
        }
        if (remains) {
            result.add(Interval(left, right, left_closed, right_closed));
        }
    }
    return result;
}

bool IntervalSet::operator==(const IntervalSet& other) const {
    if (size_ != other.size_) {
        return false;
    }
    for (std::size_t i = 0; i < size_; ++i) {
        if (!(data_[i] == other.data_[i])) {
            return false;
        }
    }
    return true;
}

bool IntervalSet::operator!=(const IntervalSet& other) const {
    return !(*this == other);
}

void IntervalSet::swap(IntervalSet& other) noexcept {
    std::swap(data_, other.data_);
    std::swap(size_, other.size_);
    std::swap(capacity_, other.capacity_);
}

std::ostream& operator<<(std::ostream& out, const IntervalSet& set) {
    if (set.empty()) {
        return out << "{}";
    }
    out << "{ ";
    for (std::size_t i = 0; i < set.size_; ++i) {
        if (i != 0) {
            out << ", ";
        }
        out << set.data_[i];
    }
    return out << " }";
}

void IntervalSet::Grow() {
    const std::size_t new_capacity = capacity_ == 0 ? 1 : capacity_ * 2;
    Interval* new_data = new Interval[new_capacity];
    for (std::size_t i = 0; i < size_; ++i) {
        new_data[i] = data_[i];
    }
    delete[] data_;
    data_ = new_data;
    capacity_ = new_capacity;
}

std::size_t IntervalSet::Find(const Interval& interval) const {
    std::size_t left = 0;
    std::size_t right = size_;
    while (left < right) {
        const std::size_t middle = left + (right - left) / 2;
        if (data_[middle] < interval) {
            left = middle + 1;
        } else {
            right = middle;
        }
    }
    return left < size_ && data_[left] == interval ? left : size_;
}

}  // namespace lab03
