#pragma once

namespace axiom::config {

// ========== ONLY EDIT THIS FILE ==========
// Negative motor ports reverse that motor. Set ports and robot-specific values here.
struct Ports {
    int left_front{1};
    int left_middle{2};
    int left_back{3};
    int right_front{-4};
    int right_middle{-5};
    int right_back{-6};
    int imu{7};
    int intake{10};
    int elevator_left{11};
    int elevator_right{12};
    char elevator_bottom{'A'};
    char elevator_top{'B'};
};

struct Geometry {
    double wheel_diameter_in{3.25};
    double track_width_in{12.0};
    double gear_ratio{1.0}; // motor rotations per wheel rotation
    int motor_rpm{600};
};

struct DriverFeel {
    int deadzone{8};
    double expo{0.25}; // 0 = linear, 1 = cubic
};

struct MotionGains {
    double drive_kp{0.8}, drive_kd{0.02};
    double turn_kp{1.8}, turn_kd{0.05};
};

struct RobotConfig {
    Ports ports{};
    Geometry geometry{};
    DriverFeel driver{};
    MotionGains motion{};
};

inline constexpr RobotConfig CURRENT{};

} // namespace axiom::config
