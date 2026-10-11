#include "axiom/robot/robot.hpp"
#include "axiom/config/robot_config.hpp"

namespace axiom::robot {
    Drive::Drive(const config::RobotConfig& c) :
        lf_(c.drive.left.front),
        lm_(c.drive.left.middle),
        lb_(c.drive.left.back),
        rf_(c.drive.right.front),
        rm_(c.drive.right.middle),
        rb_(c.drive.right.back),
        imu_(c.drive.imu_port)
    {
        lf.set_break_mode(pros::E_MOTOR_BRAKE_BRAKE);
        lm.set_break_mode(pros::E_MOTOR_BRAKE_BRAKE);
        lb.set_break_mode(pros::E_MOTOR_BRAKE_BRAKE);
        rf.set_break_mode(pros::E_MOTOR_BRAKE_BRAKE);
        rm.set_break_mode(pros::E_MOTOR_BRAKE_BRAKE);
        rb.set_break_mode(pros::E_MOTOR_BRAKE_BRAKE);
    }

    void Drive::arcade(int forward, int turn) {
        tank(forward + turn, forward - turn);
    }
    void Drive::tank(int left, int right) {
        lf.move(left);
        lm.move(left);
        lb.move(left);
        rf.move(right);
        rm.move(right);
        rb.move(right);
    }
    void Drive::stop(pros::motor_brake_mode_e_t mode) {
        for (auto& m: {&lf_, &lm_, &lb_, &rf_, &rm_, &rb_})
        m->set_brake_mode(mode);
        tank(0, 0);
    }
    void Drive::forward_time(int velocity, int ms) {
        tank(velocity, velocity);
        pros::delay(ms);
        stop();
    }
    void Drive::turn_time(int velocity, int ms) {
        tank(velocity, -velocity);
        pros::delay(ms);
        stop();
    }
    bool Drive::imu_ready() const {
        return !imu_.is_calibrating();
    }
    double Drive::left_temperature() const {
        return (lf_.get_temperature() + lm_.get_temperature() + lb_.get_temperature())/3.0;
    }
    double Drive::right_temperature() const {
        return (rf_.get_temperature() + rm_.get_temperature() + rb_.get_temperature())/3.0;
    }
    Robot::Robot(const config::RobotConfig& c) : drive_(c), intake_(c.intake_port), elevator_(c.elevator_left_port, c.elevator_right_port, c.elevator_bottom, c.elevator_top), config_(c) {}
    void Robot::initialize() {
        drive.imu().reset()
    }
    void Robot::update() {
        elevator_.update();
    }
    app::Snapshot Robot::snapshot() const {
        app::Snapshot s;
        s.imu_ready = drive_.imu_ready();
        s.elevator_homed = elevator_.homed();
        s.battery = pros::battery::get_capacity();
        s.left_temp = drive_.left_temperature();
        s.right_temp = drive_.right_temperature();
        s.intake_temp = intake_.temperature();
        s.elevator_temp = elevator_.temperature();
        s.health = s.imu_ready() ? "READY" : "IMU CALIBRATING"; return s;
        Robot& instance() {
            static Robot robot(config::LARGE);
            return robot;
        }
    }
}