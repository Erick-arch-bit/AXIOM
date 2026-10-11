#pragma once
#include "pros/rtos.hpp"

namespace axiom::control {
    class safety {
        public:
            static constexpr int kMaxVoltage = 12000;
            static int clamp_voltage(int v) {
                return v > kMaxVoltage ? kMaxVoltage : (v < -kMaxVoltage ? -kMaxVoltage : v);
            }
            static void wait_ms(int ms) {
                pros::delay(ms);
            }
    };
}