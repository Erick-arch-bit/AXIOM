#include "axiom/robot/robot.hpp"
#include "axiom/ui/ui.hpp"
#include "pros/rtos.hpp"
namespace axiom::auton {
void awp_safe(){ auto& r=robot::instance(); r.drive.forward_time(65,650); r.intake.collect(100); pros::delay(500); r.intake.stop(); r.drive.turn_time(55,420); r.drive.stop(); }
void pins_toggle(){ auto& r=robot::instance(); r.drive.forward_time(60,500); r.intake.collect(100); pros::delay(350); r.intake.stop(); }
void skills(){ auto& r=robot::instance(); r.drive.forward_time(60,600); r.drive.turn_time(55,420); r.drive.forward_time(60,600); r.drive.stop(); }
void run_selected(){ switch(ui::selected_auton()){ case app::AutonId::AwpSafe: awp_safe(); break; case app::AutonId::PinsToggle: pins_toggle(); break; case app::AutonId::Skills: skills(); break; } }
}
