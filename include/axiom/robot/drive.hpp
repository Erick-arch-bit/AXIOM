#pragma once
#include "pros/motors.hpp"
#include "pros/imu.hpp"
#include "axiom/config/robot_config.hpp"
#include "axiom/control/pid.hpp"
#include "pros/rtos.hpp"

namespace axiom::robot {
    class Drive {
        public:
            explicit Drive(const config::RobotConfig& c);
            void arcade(int forward, int turn);
            void tank(int left, int right);
            void stop(pros::motor_brake_mode_e_t mode = pros::E_MOTOR_BRAKE_BRAKE);
            void forward(double inches);
            void turn(double degrees);
            bool calibrate_imu(uint32_t timeout_ms);
            void forward_time(int velocity, int milliseconds);
            void turn_time(int velocity, int milliseconds);
            bool imu_ready() const;
            bool imu_calibration_failed() const { return imu_calibration_failed_; }
            double left_temperature() const;
            double right_temperature() const;
            double heading() const { return imu_.get_heading(); }
            pros::Imu& imu() { return imu_; }
        private:
            pros::Motor lf_, lm_, lb_, rf_, rm_, rb_;
            pros::Imu imu_;
            config::Geometry geometry_;
            config::MotionGains gains_;
            bool imu_ready_{false};
            bool imu_calibration_failed_{false};
    };
}
