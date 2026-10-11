#include "axiom/robot/robot.hpp"
#include "pros/rtos.hpp"
namespace axiom::driver {
static pros::Controller master(pros::E_CONTROLLER_MASTER);
void run(){ auto& r=robot::instance(); while(true){ int f=master.get_analog(pros::E_CONTROLLER_ANALOG_LEFT_Y); int t=master.get_analog(pros::E_CONTROLLER_ANALOG_RIGHT_X); r.drive.arcade(f,t); if(master.get_digital(pros::E_CONTROLLER_DIGITAL_R1)) r.intake.collect(); else if(master.get_digital(pros::E_CONTROLLER_DIGITAL_R2)) r.intake.release(); else r.intake.stop(); int e=0; if(master.get_digital(pros::E_CONTROLLER_DIGITAL_L1))e=100; if(master.get_digital(pros::E_CONTROLLER_DIGITAL_L2))e=-100; if(e)r.elevator.manual(e); else r.elevator.stop(); r.update(); pros::delay(20); } }
}
