# AXIOM PROS Template

## 30-second setup

1. Open [`include/axiom/config/robot_config.hpp`](include/axiom/config/robot_config.hpp).
2. Edit the motor, IMU, and elevator switch ports in `config::CURRENT.ports` (use a negative motor port to reverse a motor).
3. Set wheel diameter, track width, and motor RPM in `config::CURRENT.drive`.
4. Build and upload with the PROS extension or `pros mu` commands.
5. Select a routine on the Brain's AUTON page, then run the program.

The driver map is in `src/driver.cpp`: left stick Y and right stick X drive arcade; R1/R2 run the intake forward/reverse; L1/L2 move the elevator up/down. Joystick deadzone and expo are configurable in the same robot config header.

The included autonomous routines are intentionally short examples. Tune their distances and turns for your robot and field strategy before competition. Distance and turn helpers use motor position and the IMU with bounded PID-style control; wheel slip, gearing, motor cartridge, and IMU mounting affect accuracy.

## Current scope

AXIOM currently provides a six-motor tank base, basic distance/heading motion, driver arcade control, an LVGL auton selector, and live battery/temperature/IMU/elevator diagnostics. It does not include odometry, pose control, pure pursuit, LemLib/EZ-Template integration, or selected-auton SD persistence yet.
