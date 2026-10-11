#pragma once
#include "axiom/config/robot_config.hpp"
#include "axiom/robot/drive.hpp"
#include "axiom/subsystems/intake.hpp"
#include "axiom/subsystems/elevator.hpp"
#include "axiom/app/app_state.hpp"

namespace axiom::robot {
    class Robot {
        public :
            explicit Robot(const config::RobotConfig& c);
            void initialize();
            void update();
            app::Snapshot snapshot() const;
            void disabled();
            Drive drive;
            subsystems::Intake intake;
            subsystems::Elevator elevator;
        private:
            config::RobotConfig config_;
    };
    Robot& instance();
}
