#include <cmath>
#include <iostream>

#include "lab03/interval.hpp"
#include "lab03/interval_set.hpp"

namespace {

void ClearInput() {
    std::cin.clear();
    char symbol{};
    while (std::cin.get(symbol) && symbol != '\n') {
    }
    std::cout << "Ошибка ввода.\n";
}

bool ReadInterval(lab03::Interval& interval) {
    double left{};
    double right{};
    std::cout << "Границы отрезка l r (l <= r): ";
    if (!(std::cin >> left >> right) || !std::isfinite(left) || !std::isfinite(right) || left > right) {
        if (!std::cin.eof() && !std::cin.bad()) {
            ClearInput();
        }
        return false;
    }
    interval = lab03::Interval(left, right);
    return true;
}

}  // namespace

int main() {
    lab03::IntervalSet first;
    lab03::IntervalSet second;
    while (true) {
        std::cout
            << "\nA = " << first << "\nB = " << second
            << "\n1. Добавить в A\n2. Добавить в B\n3. Удалить из A\n4. Удалить из B\n"
               "5. Проверить наличие в A\n6. A | B\n7. A & B\n8. A - B\n9. A == B\n0. Выход\nКоманда: ";
        int command{};
        if (!(std::cin >> command)) {
            if (std::cin.eof() || std::cin.bad()) {
                return 0;
            }
            ClearInput();
            continue;
        }
        switch (command) {
            case 0:
                return 0;
            case 1:
            case 2:
            case 3:
            case 4:
            case 5: {
                lab03::Interval interval;
                if (!ReadInterval(interval)) {
                    break;
                }
                bool changed{};
                if (command == 1) {
                    changed = first.add(interval);
                } else if (command == 2) {
                    changed = second.add(interval);
                } else if (command == 3) {
                    changed = first.remove(interval);
                } else if (command == 4) {
                    changed = second.remove(interval);
                } else {
                    std::cout << (first(interval) ? "Есть такой элемент.\n" : "Нет такого элемента.\n");
                    break;
                }
                std::cout << (changed ? "Множество изменилось.\n" : "Множество не изменилось.\n");
                break;
            }
            case 6:
                std::cout << "A | B = " << (first | second) << '\n';
                break;
            case 7:
                std::cout << "A & B = " << (first & second) << '\n';
                break;
            case 8:
                std::cout << "A - B = " << (first - second) << '\n';
                break;
            case 9:
                std::cout << (first == second ? "Множества равны.\n" : "Множества различаются.\n");
                break;
            default:
                std::cout << "Нет такой команды.\n";
                break;
        }
    }
}
