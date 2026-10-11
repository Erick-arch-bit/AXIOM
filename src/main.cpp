#include "main.h"
#include "axiom/robot/robot.hpp"
#include "axiom/ui/ui.hpp"
namespace axiom::auton { void run_selected(); }
namespace axiom::driver { void run(); }
void initialize(){ auto& r=axiom::robot::instance(); r.initialize(); axiom::ui::init(); }
void competition_initialize(){ axiom::ui::init(); }
void autonomous(){ axiom::auton::run_selected(); }
void opcontrol(){ axiom::driver::run(); }
void disabled(){ axiom::robot::instance().disabled(); }
