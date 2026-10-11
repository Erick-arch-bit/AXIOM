#include "axiom/subsystems/elevator.hpp"
namespace axiom::subsystems {
Elevator::Elevator(int l,int r,char b,char t):left_(l),right_(r),bottom_(b),top_(t){ left_.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD); right_.set_brake_mode(pros::E_MOTOR_BRAKE_HOLD); }
void Elevator::manual(int p){ if(p>0 && top_.get_value()) p=0; if(p<0 && bottom_.get_value()) {p=0; homed_=true; state_=ElevatorState::Bottom;} left_.move(p); right_.move(p); if(p!=0) state_=ElevatorState::Manual; }
void Elevator::to_low(){ if(homed_) manual(45); }
void Elevator::to_high(){ if(!top_.get_value()) manual(90); else stop(); }
void Elevator::home(){ state_=ElevatorState::Homing; while(!bottom_.get_value()){ manual(-45); pros::delay(10); } stop(); homed_=true; state_=ElevatorState::Bottom; }
void Elevator::stop(){left_.brake();right_.brake();}
void Elevator::update(){ if(bottom_.get_value()){homed_=true; if(state_==ElevatorState::Homing) state_=ElevatorState::Bottom;} if(top_.get_value()) stop(); }
double Elevator::temperature() const{return (left_.get_temperature()+right_.get_temperature())/2.0;}
}
