#pragma once

#include "pros/adi.hpp"
#include "pros/motors.hpp"
#include "pros/rtos.hpp"
#include <cmath>
#include <cstdint>

namespace axiom::control {

class SimpleMotor {
public:
    explicit SimpleMotor(int port, pros::motor_brake_mode_e_t brake = pros::E_MOTOR_BRAKE_COAST)
        : motor_(port) { motor_.set_brake_mode(brake); }
    void run(int power = 127) { motor_.move(power); }
    void reverse(int power = 127) { motor_.move(-power); }
    void stop() { motor_.move(0); }
    void run_for(int power, int ms) { run(power); pros::delay(ms); stop(); }
    void reverse_for(int power, int ms) { reverse(power); pros::delay(ms); stop(); }
    double temperature() const { return motor_.get_temperature(); }
    pros::Motor& raw() { return motor_; }
protected:
    pros::Motor motor_;
};

class DualMotor {
public:
    DualMotor(int left, int right, pros::motor_brake_mode_e_t brake = pros::E_MOTOR_BRAKE_HOLD)
        : left_(left), right_(right) { left_.set_brake_mode(brake); right_.set_brake_mode(brake); }
    void run(int power = 127) { left_.move(power); right_.move(power); }
    void reverse(int power = 127) { run(-power); }
    void stop() { left_.move(0); right_.move(0); }
    void run_for(int power, int ms) { run(power); pros::delay(ms); stop(); }
    double temperature() const { return (left_.get_temperature() + right_.get_temperature()) * 0.5; }
protected:
    pros::Motor left_, right_;
};

class MotorWithLimits : public DualMotor {
public:
    MotorWithLimits(int left, int right, char bottom, char top)
        : DualMotor(left, right), bottom_(static_cast<uint8_t>(bottom)), top_(static_cast<uint8_t>(top)) {}
    bool at_bottom() const { return bottom_.get_value(); }
    bool at_top() const { return top_.get_value(); }
    bool homed() const { return homed_; }
    void manual(int power) {
        if (power > 0 && at_top()) power = 0;
        if (power < 0 && at_bottom()) { power = 0; homed_ = true; }
        run(power);
    }
    void home(int power = -60, uint32_t timeout_ms = 3000) {
        uint32_t elapsed = 0;
        while (!at_bottom() && elapsed < timeout_ms) { manual(power); pros::delay(10); elapsed += 10; }
        stop();
        if (at_bottom()) homed_ = true;
    }
    void to_top(int power = 90, uint32_t timeout_ms = 3000) {
        uint32_t elapsed = 0;
        while (!at_top() && elapsed < timeout_ms) { manual(power); pros::delay(10); elapsed += 10; }
        stop();
    }
    void to_bottom(int power = -60, uint32_t timeout_ms = 3000) { home(power, timeout_ms); }
    void update() { if (at_bottom()) homed_ = true; }
protected:
    pros::adi::DigitalIn bottom_, top_;
    bool homed_{false};
};

class PositionMotor {
public:
    explicit PositionMotor(int port, double ticks_per_degree = 1.0)
        : motor_(port), ticks_per_degree_(ticks_per_degree > 0 ? ticks_per_degree : 1.0) {
        motor_.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD);
        motor_.tare_position();
    }
    void manual(int power) { motor_.move(power); }
    void stop() { motor_.move(0); }
    double degrees() const { return motor_.get_position() / ticks_per_degree_; }
    void to(double target_deg, double kp = 2.0, uint32_t timeout_ms = 2000) {
        uint32_t elapsed = 0; int settled = 0;
        while (elapsed < timeout_ms && settled < 8) {
            const double error = target_deg - degrees();
            if (std::fabs(error) < 2.0) { ++settled; stop(); }
            else { settled = 0; int power = static_cast<int>(kp * error); if (power > 127) power = 127; if (power < -127) power = -127; if (std::abs(power) < 15) power = error > 0 ? 15 : -15; manual(power); }
            pros::delay(10); elapsed += 10;
        }
        stop();
    }
    double temperature() const { return motor_.get_temperature(); }
private:
    pros::Motor motor_;
    double ticks_per_degree_;
};

} // namespace axiom::control
