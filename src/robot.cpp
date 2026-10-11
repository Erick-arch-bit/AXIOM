#include "axiom/robot/robot.hpp"
#include "lemlib/api.hpp"
#include <algorithm>
#include <cmath>
#include <cstdint>

namespace axiom::robot {
namespace {
constexpr double PI = 3.14159265358979323846;
double wrap_heading(double degrees) {
    degrees = std::fmod(degrees, 360.0);
    return degrees < 0 ? degrees + 360.0 : degrees;
}
uint32_t forward_timeout(double inches, const config::Geometry& geometry) {
    if (geometry.wheel_diameter_in <= 0 || geometry.gear_ratio <= 0) return 4000;
    const double rpm = geometry.motor_rpm > 0 ? geometry.motor_rpm : 200.0;
    const double wheel_rpm = rpm / geometry.gear_ratio;
    const double estimated_ms = std::abs(inches) / (PI * geometry.wheel_diameter_in) * 60000.0 / wheel_rpm;
    return static_cast<uint32_t>(std::min(10000.0, std::max(1500.0, estimated_ms * 2.5)));
}
pros::MotorGears motor_gears_for_rpm(int rpm) {
    if (rpm <= 100) return pros::MotorGears::red;
    if (rpm <= 200) return pros::MotorGears::green;
    return pros::MotorGears::blue;
}
}

struct Drive::Backend {
    lemlib::Drivetrain drivetrain;
    lemlib::TrackingWheel left_tracking;
    lemlib::TrackingWheel right_tracking;
    lemlib::OdomSensors sensors;
    lemlib::ControllerSettings linear;
    lemlib::ControllerSettings angular;
    lemlib::Chassis chassis;

    Backend(pros::MotorGroup& left, pros::MotorGroup& right, pros::Imu& imu,
            const config::Geometry& geometry, const config::MotionGains& gains)
        : drivetrain(&left, &right, geometry.track_width_in, geometry.wheel_diameter_in,
                     geometry.motor_rpm / (geometry.gear_ratio > 0 ? geometry.gear_ratio : 1.0),
                     geometry.horizontal_drift),
          left_tracking(&left, geometry.wheel_diameter_in, -geometry.track_width_in / 2.0,
                        geometry.motor_rpm / (geometry.gear_ratio > 0 ? geometry.gear_ratio : 1.0)),
          right_tracking(&right, geometry.wheel_diameter_in, geometry.track_width_in / 2.0,
                         geometry.motor_rpm / (geometry.gear_ratio > 0 ? geometry.gear_ratio : 1.0)),
          sensors(&left_tracking, &right_tracking, nullptr, nullptr, &imu),
          linear(gains.drive_kp, 0, gains.drive_kd, 3, 1, 100, 3, 500, 20),
          angular(gains.turn_kp, 0, gains.turn_kd, 3, 1, 100, 3, 500, 0),
          chassis(drivetrain, linear, angular, sensors) {}
};

Drive::Drive(const config::RobotConfig& c)
    : left_motors_({static_cast<int8_t>(c.ports.left_front), static_cast<int8_t>(c.ports.left_middle), static_cast<int8_t>(c.ports.left_back)}, motor_gears_for_rpm(c.geometry.motor_rpm)),
      right_motors_({static_cast<int8_t>(c.ports.right_front), static_cast<int8_t>(c.ports.right_middle), static_cast<int8_t>(c.ports.right_back)}, motor_gears_for_rpm(c.geometry.motor_rpm)),
      imu_(c.ports.imu), geometry_(c.geometry), gains_(c.motion),
      backend_(new Backend(left_motors_, right_motors_, imu_, geometry_, gains_)) {
    left_motors_.set_brake_mode(pros::E_MOTOR_BRAKE_BRAKE);
    right_motors_.set_brake_mode(pros::E_MOTOR_BRAKE_BRAKE);
}
Drive::~Drive() = default;

void Drive::arcade(int forward, int turn) { backend_->chassis.arcade(forward, turn, true); }
void Drive::tank(int left, int right) { backend_->chassis.tank(left, right, true); }
void Drive::stop(pros::motor_brake_mode_e_t mode) {
    if (odom_started_) backend_->chassis.cancelMotion();
    backend_->chassis.setBrakeMode(mode);
    left_motors_.move(0);
    right_motors_.move(0);
}
void Drive::forward_time(int velocity, int milliseconds) {
    tank(velocity, velocity); pros::delay(milliseconds); stop();
}
void Drive::turn_time(int velocity, int milliseconds) {
    tank(velocity, -velocity); pros::delay(milliseconds); stop();
}
void Drive::forward(double inches) {
    if (!imu_ready() || inches == 0) { stop(); return; }
    const DrivePose pose = get_pose();
    // LemLib pose uses inches and degrees; zero faces +Y and positive heading is clockwise.
    const double radians = pose.heading * PI / 180.0;
    move_to_point(pose.x + inches * std::sin(radians), pose.y + inches * std::cos(radians), forward_timeout(inches, geometry_));
}
void Drive::turn(double degrees) {
    if (!imu_ready() || !odom_started_) { stop(); return; }
    const DrivePose pose = get_pose();
    const double target = wrap_heading(pose.heading + degrees);
    const int timeout = static_cast<int>(std::min(6000.0, std::max(1200.0, std::abs(degrees) * 18.0)));
    backend_->chassis.turnToHeading(target, timeout, {}, false);
}
bool Drive::calibrate_imu(uint32_t timeout_ms) {
    imu_ready_ = false;
    imu_calibration_failed_ = false;
    if (imu_.reset() < 0) { imu_calibration_failed_ = true; return false; }
    uint32_t elapsed = 0;
    while (imu_.is_calibrating() && elapsed < timeout_ms) { pros::delay(10); elapsed += 10; }
    imu_ready_ = !imu_.is_calibrating();
    imu_calibration_failed_ = !imu_ready_;
    if (imu_ready_ && !odom_started_) {
        backend_->chassis.calibrate(false); // The IMU was calibrated above; start LemLib odometry once.
        backend_->chassis.setPose(0, 0, imu_.get_heading());
        odom_started_ = true;
    }
    return imu_ready_;
}
bool Drive::imu_ready() const { return imu_ready_ && !imu_.is_calibrating(); }
void Drive::set_pose(double x, double y, double heading_deg) {
    if (odom_started_) backend_->chassis.setPose(x, y, wrap_heading(heading_deg));
}
DrivePose Drive::get_pose() const {
    if (!odom_started_) return {0, 0, heading()};
    const lemlib::Pose pose = backend_->chassis.getPose();
    return {pose.x, pose.y, pose.theta};
}
void Drive::move_to_point(double x, double y, uint32_t timeout_ms) {
    if (!imu_ready() || !odom_started_) { stop(); return; }
    backend_->chassis.moveToPoint(x, y, static_cast<int>(timeout_ms), {}, false);
}
void Drive::move_to_pose(double x, double y, double heading_deg, uint32_t timeout_ms) {
    if (!imu_ready() || !odom_started_) { stop(); return; }
    backend_->chassis.moveToPose(x, y, wrap_heading(heading_deg), static_cast<int>(timeout_ms), {}, false);
}
double Drive::left_temperature() const {
    const auto temperatures = left_motors_.get_temperature_all();
    if (temperatures.empty()) return 0;
    double total = 0; for (double value : temperatures) total += value;
    return total / temperatures.size();
}
double Drive::right_temperature() const {
    const auto temperatures = right_motors_.get_temperature_all();
    if (temperatures.empty()) return 0;
    double total = 0; for (double value : temperatures) total += value;
    return total / temperatures.size();
}

Robot::Robot(const config::RobotConfig& c)
    : drive(c), intake(c.ports.intake),
      elevator(c.ports.elevator_left, c.ports.elevator_right, c.ports.elevator_bottom, c.ports.elevator_top), config_(c) {}
void Robot::initialize() {
    drive.stop(pros::E_MOTOR_BRAKE_BRAKE);
    drive.calibrate_imu(5000);
}
void Robot::update() { elevator.update(); }
void Robot::disabled() { drive.stop(pros::E_MOTOR_BRAKE_BRAKE); intake.stop(); elevator.stop(); }
app::Snapshot Robot::snapshot() const {
    app::Snapshot s;
    s.imu_ready = drive.imu_ready(); s.imu_calibration_failed = drive.imu_calibration_failed(); s.elevator_homed = elevator.homed();
    s.battery = pros::battery::get_capacity(); s.left_temp = drive.left_temperature();
    s.right_temp = drive.right_temperature(); s.intake_temp = intake.temperature();
    s.elevator_temp = elevator.temperature();
    const DrivePose pose = drive.get_pose();
    s.pose = {pose.x, pose.y, pose.heading};
    s.health = s.imu_calibration_failed ? "IMU CALIBRATION FAILED" : (!s.imu_ready ? "IMU CALIBRATING" : (s.elevator_homed ? "READY" : "ELEVATOR NOT HOMED"));
    return s;
}
Robot& instance() { static Robot robot(config::CURRENT); return robot; }
}
