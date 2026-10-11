#pragma once
#include "axiom/control/motors.hpp"

namespace axiom::subsystems {
class Intake : public control::SimpleMotor {
public:
    explicit Intake(int port) : SimpleMotor(port, pros::E_MOTOR_BRAKE_COAST) {}
    void collect(int power = 127) { run(power); }
    void release(int power = 127) { reverse(power); }
    void collect_for(int ms, int power = 127) { run_for(power, ms); }
    void release_for(int ms, int power = 127) { reverse_for(power, ms); }
};
}
