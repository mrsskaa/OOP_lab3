#pragma once

#include <iostream>

namespace lab03 {

class Interval {
public:
    Interval() = default;
    Interval(double left, double right, bool left_closed = true, bool right_closed = true);

    double left() const;
    double right() const;
    bool left_closed() const;
    bool right_closed() const;

    bool operator==(const Interval& other) const;
    bool operator<(const Interval& other) const;

    friend std::ostream& operator<<(std::ostream& out, const Interval& interval);

private:
    double left_ = 0;
    double right_ = 0;
    bool left_closed_ = true;
    bool right_closed_ = true;
};

}  // namespace lab03
