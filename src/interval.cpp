#include "lab03/interval.hpp"

#include <cassert>
#include <cmath>

namespace lab03 {

Interval::Interval(double left, double right, bool left_closed, bool right_closed)
    : left_(left), right_(right), left_closed_(left_closed), right_closed_(right_closed) {
    assert(std::isfinite(left) && std::isfinite(right));
    assert(left < right || (left == right && left_closed && right_closed));
}

double Interval::left() const {
    return left_;
}

double Interval::right() const {
    return right_;
}

bool Interval::left_closed() const {
    return left_closed_;
}

bool Interval::right_closed() const {
    return right_closed_;
}

bool Interval::operator==(const Interval& other) const {
    return left_ == other.left_ && right_ == other.right_ && left_closed_ == other.left_closed_ &&
           right_closed_ == other.right_closed_;
}

bool Interval::operator<(const Interval& other) const {
    if (left_ != other.left_) {
        return left_ < other.left_;
    }
    if (left_closed_ != other.left_closed_) {
        return left_closed_;
    }
    if (right_ != other.right_) {
        return right_ < other.right_;
    }
    return !right_closed_ && other.right_closed_;
}

std::ostream& operator<<(std::ostream& out, const Interval& interval) {
    return out << (interval.left_closed_ ? '[' : '(') << interval.left_ << ", " << interval.right_
               << (interval.right_closed_ ? ']' : ')');
}

}  // namespace lab03
