#pragma once
#include "pros/motors.hpp"

namespace axiom::subsystems {
    class Intake {
        public:
            explicit Intake(int port) : motor_(port) {}
            void collect(int velocity = 127);
            void release(int velocity = 127);
            void stop();
            double temperature() const { return motor_.get_temperature(); }
        private:
            pros::Motor motor_;
    };
}