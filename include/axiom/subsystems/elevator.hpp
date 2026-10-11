#pragma once
#include "axiom/control/motors.hpp"

namespace axiom::subsystems {
class Elevator : public control::MotorWithLimits {
public:
    Elevator(int left, int right, char bottom, char top)
        : MotorWithLimits(left, right, bottom, top) {}
    void up(int power = 90) { manual(power); }
    void down(int power = -60) { manual(power); }
    void to_high() { to_top(); }
    void to_low() { to_bottom(); }
};
}
