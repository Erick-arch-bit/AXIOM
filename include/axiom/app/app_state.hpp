#pragma once
#include <cstdint>
namespace axiom::app{
    enum class Mode {
        Boot,
        Disabled,
        Autonomous,
        Driver
    };
    enum class AutoId {
        AwpSafe,
        PinsToggle,
        Skills
    };
    struct Snapshot {
        Mode mode{Mode::Boot};
        bool imu_ready{false};
        bool elevator_homed{false};
        double battery{0};
        double left_temp{0};
        double right_temp{0};
        Pose pose{};
        AutoId auton{AutoId::AwpSafe};
        const char * health{"BOOT"};
    };
}