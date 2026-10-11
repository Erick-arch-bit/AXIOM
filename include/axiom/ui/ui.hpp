#pragma once
#include "axiom/app/app_state.hpp"
namespace axiom::ui {
    enum class Page {Home, Autonomus, Diagnostics };
    void init();
    void show(Page page);
    void set_snapshot(const app::Snapshot& snapshot);
    Page corrent_page();
    app::AutinId selected_auton();
}