#pragma once
#include "axiom/app/app_state.hpp"
namespace axiom::ui {
enum class Page { Home, Autonomous, Diagnostics };
void init();
void show(Page page);
void set_snapshot(const app::Snapshot& snapshot);
Page current_page();
app::AutonId selected_auton();
}
