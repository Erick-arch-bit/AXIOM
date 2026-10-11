#pragma once
#include "axiom/app/app_state.hpp"
#include "axiom/robot/robot.hpp"

namespace axiom::app {
    class Controller {
        public:
            explicit Controller(robot::Robot& robot) : robot_(robot) {}
            void set_mode(Mode mode) { mode_=mode; }
            void tick() { robot_.update(); snapshot_=robot_.snapshot(); snapshot_.mode=mode_; }
            const Snapshot& snapshot() const { return snapshot_; }
        private: 
            robot::Robot& robot_; Mode mode_{Mode::Boot}; 
        Snapshot snapshot_{};
    };
}