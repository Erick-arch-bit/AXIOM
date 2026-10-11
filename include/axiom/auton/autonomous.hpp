#pragma once

#include "axiom/app/app_state.hpp"
#include <cstddef>

namespace axiom::auton {
struct AutonEntry { app::AutonId id; const char* name; const char* description; void (*fn)(); };
const AutonEntry* entries();
std::size_t count();
void run_selected();
}
