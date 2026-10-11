# AXIOM PROS Template

## 30-second setup

1. Open [`include/axiom/config/robot_config.hpp`](include/axiom/config/robot_config.hpp).
2. Edit the motor, IMU, and elevator switch ports in `config::CURRENT.ports` (use a negative motor port to reverse a motor).
3. Set wheel diameter, track width, gear ratio, motor RPM, driver feel, and motion gains in `config::CURRENT`.
4. Build with `pros make`, then upload with `pros upload` (or use the PROS extension actions).
5. Select a routine on the Brain's AUTON page, then run the program.

## Chassis backend

`axiom::robot::Drive` keeps the public AXIOM API and delegates motion and odometry to LemLib **0.5.6**, pinned in `project.pros`. The project uses PROS kernel 4.2.2; LemLib's installed template supports the 4.2.1 kernel line. LemLib types stay inside `src/robot.cpp`, so `main.cpp`, mechanisms, and auton routines only call `r.drive.*`.

This repository already contains the pinned LemLib headers and library, so a clone does not need a separate dependency install. To add the official stable depot and apply LemLib in another PROS project, use:

```sh
pros c add-depot LemLib https://raw.githubusercontent.com/LemLib/LemLib/depot/stable.json
pros c apply LemLib
```

Those depot commands apply its current stable release; AXIOM's checked-in dependency remains pinned at 0.5.6 until deliberately updated. LemLib receives wheel RPM (`motor_rpm / gear_ratio`), track width, and wheel diameter in inches. `motor_rpm` should match the fitted 100, 200, or 600 RPM V5 cartridge; AXIOM sets that PROS motor gearset from this value. `gear_ratio` means motor rotations per wheel rotation. Headings and relative turns are in degrees, with positive turns clockwise. The odometry uses the IMU and LemLib tracking wheels backed by the left and right drive motor groups; adding passive tracking wheels may improve accuracy.

The driver map is in `src/driver.cpp`: left stick Y and right stick X drive arcade; R1/R2 run the intake forward/reverse; L1/L2 move the elevator up/down. Joystick deadzone and expo are configurable in the same robot config header.

AXIOM owns driver feel: it applies the configured deadzone and expo in `driver.cpp`, then calls LemLib arcade with its input curve disabled. This keeps each stick value shaped once. There is no tank-mode configuration yet.

## Add a mechanism in 3 steps

1. Add its smart-port/ADI entries to `Ports` in `robot_config.hpp`.
2. Add a small game-named subsystem in `include/axiom/subsystems/` using a helper from `include/axiom/control/motors.hpp`.
3. Add the subsystem as a public member of `Robot`, construct it from the configured ports, and bind its controls in `src/driver.cpp` (then use it in autons if needed).

The included autonomous routines are intentionally short examples. `AWP Safe`, `Pins Toggle`, and `Skills` show relative `forward(inches)` and `turn(degrees)` calls. `Pose Demo` shows pose control:

```cpp
r.drive.set_pose(0, 0, 0);
r.drive.move_to_point(24, 0);
r.drive.move_to_pose(24, 24, 90);
r.drive.stop();
```

The pose frame uses inches, with heading in degrees, zero pointing along +Y, and positive heading clockwise. Measure wheel diameter and track width on the robot, then tune `drive_kp`, `drive_kd`, `turn_kp`, and `turn_kd`; `horizontal_drift` starts at about 2 for omni wheels and 8 for traction wheels. LemLib odometry currently uses drive motor encoders and the IMU, so wheel slip affects position. IMU calibration has a five-second timeout; failure is shown as **IMU CALIBRATION FAILED** and motion calls safely do nothing.

The selected routine is saved to `/usd/axiom_auton.txt` when an SD card is available. If the card or file is missing, AXIOM keeps the default routine and continues normally.

## Current scope

AXIOM currently provides a six-motor tank base, LemLib-backed distance/heading motion and motor-encoder odometry, driver arcade control, an LVGL auton selector with optional SD persistence, and live battery/temperature/IMU/elevator/pose diagnostics. Passive tracking-wheel odometry, path-file pure pursuit, holonomic drive, and live tuning are outside the current template. Motion accuracy depends on measured geometry, tuned gains, and limiting wheel slip.

## Credits

AXIOM uses [PROS](https://pros.cs.purdue.edu/), [LemLib 0.5.6](https://lemlib.readthedocs.io/en/stable/), and the bundled [LVGL](https://lvgl.io/) user interface library.
