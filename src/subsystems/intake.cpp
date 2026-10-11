#include "axiom/subsystems/intake.hpp"
namespace axiom::subsystems { void Intake::collect(int v){motor_.move(v);} void Intake::release(int v){motor_.move(-v);} void Intake::stop(){motor_.brake();} }
