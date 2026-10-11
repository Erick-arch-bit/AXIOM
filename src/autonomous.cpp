#include "axiom/auton/autonomous.hpp"
#include "axiom/robot/robot.hpp"
#include "axiom/ui/ui.hpp"

namespace axiom::auton {
namespace {
void awp_safe() {
    auto& r = robot::instance();
    r.drive.forward(18);
    r.intake.collect_for(400, 100);
    r.drive.turn(45);
    r.elevator.to_high();
    r.drive.forward(12);
    r.intake.release_for(300);
    r.elevator.to_low();
}
void pins_toggle() {
    auto& r = robot::instance();
    r.drive.forward(16);
    r.intake.collect_for(350, 100);
    r.drive.stop();
}
void skills() {
    auto& r = robot::instance();
    r.drive.forward(24);
    r.drive.turn(90);
    r.drive.forward(24);
    r.drive.turn(-90);
    r.drive.stop();
}
const AutonEntry routines[] = {
    {app::AutonId::AwpSafe, "AWP Safe", "Short, conservative example", awp_safe},
    {app::AutonId::PinsToggle, "Pins Toggle", "Intake and drive example", pins_toggle},
    {app::AutonId::Skills, "Skills", "Basic multi-move example", skills},
};
}
const AutonEntry* entries() { return routines; }
std::size_t count() { return sizeof(routines) / sizeof(routines[0]); }
void run_selected() {
    const auto selected = ui::selected_auton();
    for (const auto& routine : routines) if (routine.id == selected) { routine.fn(); return; }
    routines[0].fn();
}
}
