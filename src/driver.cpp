#include "axiom/robot/robot.hpp"
#include "axiom/config/robot_config.hpp"
#include "axiom/ui/ui.hpp"
#include "pros/rtos.hpp"
#include <algorithm>
#include <cmath>

namespace axiom::driver {
namespace {
pros::Controller master(pros::E_CONTROLLER_MASTER);
int shape(int value) {
    const auto& cfg = config::CURRENT.drive;
    if (std::abs(value) <= cfg.joystick_deadzone) return 0;
    const double normalized = static_cast<double>(value) / 127.0;
    const double expo = std::max(0.0, std::min(1.0, cfg.joystick_expo));
    const double shaped = normalized * (1.0 - expo) + normalized * normalized * normalized * expo;
    return static_cast<int>(shaped * 127.0);
}
}
void run() {
    auto& r = robot::instance();
    while (true) {
        const int forward = shape(master.get_analog(pros::E_CONTROLLER_ANALOG_LEFT_Y));
        const int turn = shape(master.get_analog(pros::E_CONTROLLER_ANALOG_RIGHT_X));
        r.drive.arcade(forward, turn);
        if (master.get_digital(pros::E_CONTROLLER_DIGITAL_R1)) r.intake.collect();
        else if (master.get_digital(pros::E_CONTROLLER_DIGITAL_R2)) r.intake.release();
        else r.intake.stop();
        int elevator_power = 0;
        if (master.get_digital(pros::E_CONTROLLER_DIGITAL_L1)) elevator_power = 100;
        else if (master.get_digital(pros::E_CONTROLLER_DIGITAL_L2)) elevator_power = -100;
        if (elevator_power) r.elevator.manual(elevator_power); else r.elevator.stop();
        r.update();
        ui::set_snapshot(r.snapshot());
        pros::delay(20);
    }
}
}
