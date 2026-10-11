#pragma once
#include "pros/motors.hpp"
#include "pros/motor_group.hpp"
#include "pros/imu.hpp"
#include "axiom/config/robot_config.hpp"
#include "pros/rtos.hpp"
#include <cstdint>
#include <memory>

namespace axiom::robot {
    struct DrivePose { double x{0}; double y{0}; double heading{0}; };
    class Drive {
        public:
            explicit Drive(const config::RobotConfig& c);
            ~Drive();
            Drive(const Drive&) = delete;
            Drive& operator=(const Drive&) = delete;
            void arcade(int forward, int turn);
            void tank(int left, int right);
            void stop(pros::motor_brake_mode_e_t mode = pros::E_MOTOR_BRAKE_BRAKE);
            void forward(double inches);
            void turn(double degrees);
            bool calibrate_imu(uint32_t timeout_ms);
            void set_pose(double x, double y, double heading_deg);
            DrivePose get_pose() const;
            void move_to_point(double x, double y, uint32_t timeout_ms = 4000);
            void move_to_pose(double x, double y, double heading_deg, uint32_t timeout_ms = 5000);
            void forward_time(int velocity, int milliseconds);
            void turn_time(int velocity, int milliseconds);
            bool imu_ready() const;
            bool imu_calibration_failed() const { return imu_calibration_failed_; }
            double left_temperature() const;
            double right_temperature() const;
            double heading() const { return imu_.get_heading(); }
            pros::Imu& imu() { return imu_; }
        private:
            struct Backend;
            pros::MotorGroup left_motors_, right_motors_;
            pros::Imu imu_;
            config::Geometry geometry_;
            config::MotionGains gains_;
            std::unique_ptr<Backend> backend_;
            bool imu_ready_{false};
            bool imu_calibration_failed_{false};
            bool odom_started_{false};
    };
}
