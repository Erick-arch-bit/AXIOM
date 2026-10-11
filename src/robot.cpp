#include "axiom/robot/robot.hpp"
#include <algorithm>
#include <cmath>
#include <cstdint>

namespace axiom::robot {
namespace {
constexpr double PI = 3.14159265358979323846;
int clamp_power(double value) {
    if (value > 127) return 127;
    if (value < -127) return -127;
    return static_cast<int>(value);
}
double angle_error(double target, double current) {
    double error = std::fmod(target - current + 540.0, 360.0) - 180.0;
    return error == -180.0 ? 180.0 : error;
}
}

Drive::Drive(const config::RobotConfig& c)
    : lf_(c.ports.left_front), lm_(c.ports.left_middle), lb_(c.ports.left_back),
      rf_(c.ports.right_front), rm_(c.ports.right_middle), rb_(c.ports.right_back),
      imu_(c.ports.imu), geometry_(c.drive) {
    for (auto* motor : {&lf_, &lm_, &lb_, &rf_, &rm_, &rb_})
        motor->set_brake_mode(pros::E_MOTOR_BRAKE_BRAKE);
}

void Drive::arcade(int forward, int turn) { tank(forward + turn, forward - turn); }
void Drive::tank(int left, int right) {
    const int max = std::max({127, std::abs(left), std::abs(right)});
    left = left * 127 / max;
    right = right * 127 / max;
    lf_.move(left); lm_.move(left); lb_.move(left);
    rf_.move(right); rm_.move(right); rb_.move(right);
}
void Drive::stop(pros::motor_brake_mode_e_t mode) {
    for (auto* motor : {&lf_, &lm_, &lb_, &rf_, &rm_, &rb_}) {
        motor->set_brake_mode(mode);
        motor->move(0);
    }
}
void Drive::forward_time(int velocity, int milliseconds) {
    tank(velocity, velocity); pros::delay(milliseconds); stop();
}
void Drive::turn_time(int velocity, int milliseconds) {
    tank(velocity, -velocity); pros::delay(milliseconds); stop();
}

void Drive::forward(double inches) {
    if (geometry_.wheel_diameter_in <= 0 || inches == 0) { stop(); return; }
    const double target = inches * 360.0 / (geometry_.wheel_diameter_in * PI);
    for (auto* motor : {&lf_, &lm_, &lb_, &rf_, &rm_, &rb_}) motor->tare_position();
    const uint32_t timeout = static_cast<uint32_t>(std::min(8000.0, 1200.0 + std::abs(inches) * 90.0));
    uint32_t elapsed = 0;
    int settled = 0;
    control::Pid controller({0.8, 0.0, 0.02, 127.0, 100.0});
    while (elapsed < timeout && settled < 5) {
        double pos = 0;
        for (auto* motor : {&lf_, &lm_, &lb_, &rf_, &rm_, &rb_}) pos += motor->get_position();
        const double error = target - pos / 6.0;
        if (std::abs(error) < 4.0) { ++settled; tank(0, 0); }
        else { settled = 0; const int power = clamp_power(controller.step(error, 0.01)); tank(power, power); }
        pros::delay(10); elapsed += 10;
    }
    stop();
}

void Drive::turn(double degrees) {
    const double target = std::fmod(imu_.get_heading() + degrees + 3600.0, 360.0);
    uint32_t elapsed = 0;
    int settled = 0;
    control::Pid controller({1.8, 0.0, 0.05, 127.0, 100.0});
    while (elapsed < 5000 && settled < 6 && !imu_.is_calibrating()) {
        const double error = angle_error(target, imu_.get_heading());
        if (std::abs(error) < 1.5) { ++settled; tank(0, 0); }
        else { settled = 0; int power = clamp_power(controller.step(error, 0.01)); if (std::abs(power) < 18) power = error > 0 ? 18 : -18; tank(power, -power); }
        pros::delay(10); elapsed += 10;
    }
    stop();
}

bool Drive::imu_ready() const { return !imu_.is_calibrating(); }
double Drive::left_temperature() const { return (lf_.get_temperature()+lm_.get_temperature()+lb_.get_temperature())/3.0; }
double Drive::right_temperature() const { return (rf_.get_temperature()+rm_.get_temperature()+rb_.get_temperature())/3.0; }

Robot::Robot(const config::RobotConfig& c)
    : drive(c), intake(c.ports.intake),
      elevator(c.ports.elevator_left, c.ports.elevator_right, c.ports.elevator_bottom, c.ports.elevator_top), config_(c) {}

void Robot::initialize() {
    drive.stop(pros::E_MOTOR_BRAKE_BRAKE);
    drive.imu().reset();
    while (drive.imu().is_calibrating()) pros::delay(10);
}
void Robot::update() { elevator.update(); }
void Robot::disabled() { drive.stop(pros::E_MOTOR_BRAKE_BRAKE); intake.stop(); elevator.stop(); }
app::Snapshot Robot::snapshot() const {
    app::Snapshot s;
    s.imu_ready = drive.imu_ready(); s.elevator_homed = elevator.homed();
    s.battery = pros::battery::get_capacity(); s.left_temp = drive.left_temperature();
    s.right_temp = drive.right_temperature(); s.intake_temp = intake.temperature();
    s.elevator_temp = elevator.temperature(); s.pose.heading = drive.heading();
    s.health = !s.imu_ready ? "IMU CALIBRATING" : (s.elevator_homed ? "READY" : "ELEVATOR NOT HOMED");
    return s;
}
Robot& instance() { static Robot robot(config::CURRENT); return robot; }
}
