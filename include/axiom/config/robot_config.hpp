#pragma once

namespace axiom::config {

// ========== ONLY EDIT THIS FILE ==========
// Smart-port motor numbers may be negative to reverse that motor.
// Change ports and geometry here to match your robot.
struct Ports {
    int left_front{-1};
    int left_middle{-2};
    int left_back{-3};
    int right_front{-4};
    int right_middle{-5};
    int right_back{-6};
    int imu{7};
    int intake{10};
    int elevator_left{11};
    int elevator_right{12};
    char elevator_bottom{'A'}; // 3-wire ADI ports on the brain
    char elevator_top{'B'};
};

struct DriveGeometry {
    double wheel_diameter_in{4.125};
    double track_width_in{12.0};
    int motor_rpm{200};
    int joystick_deadzone{8};
    double joystick_expo{0.25}; // 0 = linear, 1 = cubic
};

struct RobotConfig {
    Ports ports{};
    DriveGeometry drive{};
};

inline constexpr RobotConfig CURRENT{};

} // namespace axiom::config
