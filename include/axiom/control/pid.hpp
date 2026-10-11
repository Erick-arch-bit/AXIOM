#pragma once
#include <algorithm>

namespace axiom::control {
    struct PidConfig {
        double Kp{0};
        double Ki{0};
        double Kd{0};
        double max_output{127};
        double integral_limit{100};
    };
    class Pid {
        public:
            explicit Pid(PidConfig cfg = {}) : cfg_(cfg) {}
            void reset() {
                integral_ = 0;
                previous_ = 0;
                first_ = true;
            }
            double step(double error, double dt) {
                if (dt <= 0) dt = 0.001;
                integral_ += error * dt;
                integral_ = std::clamp(integral_, -cfg_.integral_limit, cfg_.integral_limit);
                double derivative = first_ ? 0 : (error - previous_) / dt;
                previous_ = error;
                first_ = false;
                return std::clamp(cfg_.Kp * error + cfg_.Ki * integral_ + cfg_.Kd*derivative, -cfg_.max_output, cfg_.max_output);
            }
            private:
                PidConfig cfg_;
                double integral_ {0};
                double previous_ {0};
                bool first_ {true};
    };
}