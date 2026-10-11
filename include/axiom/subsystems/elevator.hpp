#pragma once
#include "pros/motors.hpp"
#include "pros/adi.hpp"

namespace axiom::subsystems {
    enum class ElevatorState {
        Unknown,
        Homing,
        Bottom,
        Manual,
        GoalLow,
        GoalHigh,
        Fault
    };
    class Elevator {
        public:
            Elevator(int left_port, int right_port, char bottom_port, char top_port);
            void manual(int power);
            void to_low();
            void to_high();
            void home();
            void stop();
            void update();
            bool homed() const { return homed_;}
            ElevatorState state() const { return state_;}
        private:
            pros::Motor left_, right_;
            pros::DigitalIn bottom_, top_;
            bool homed_{false};
            ElevatorState state_{ElevatorState::Unknown};
    };
}
