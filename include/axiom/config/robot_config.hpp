#pragma once
#include <cstdint>
namespace axiom::config {
    struct RobotConfig{
        int left_front{-1};
        int left_middle{-2};
        int left_back{-3};
        int right_front{-4};
        int right_middle{-5};
        int right_back{-6};
        int imu_port{7};
        int intake_port{10};
        char elevator_left_port{11};
        char elevator_right_port{12};
        double wheel_diameter_in{4.125};
        double wheel_rpm{200.0};
    }
    inline constexpr RobotConfig LARGE{};
}