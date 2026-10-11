#pragma once
#include <cstdint>
namespace axiom::app{
    enum class Mode {
        Boot,
        Disabled,
        Autonomous,
        Driver
    };
    enum class AutonId {
        AwpSafe,
        PinsToggle,
        Skills
    };
    struct Pose { double x{0}; double y{0}; double heading{0}; };
    struct Snapshot {
        Mode mode{Mode::Boot};
        bool imu_ready{false};
        bool elevator_homed{false};
        double battery{0};
        double left_temp{0};
        double right_temp{0};
        Pose pose{};
        AutonId auton{AutonId::AwpSafe};
        double intake_temp{0};
        double elevator_temp{0};
        const char * health{"BOOT"};
    };
}
