#include "axiom/ui/ui.hpp"
#include "axiom/ui/lvgl_compat.hpp"
#include "axiom/auton/autonomous.hpp"
#include <cstdint>
#include <cstdio>

namespace axiom::ui {
namespace {
enum : uint32_t { BG=0x10151C, PANEL=0x19232E, LINE=0x2F4354, TEXT=0xF2F6FA, MUTED=0x8FA2B5, CYAN=0x24C8D8, GREEN=0x47D18C, AMBER=0xF2B84B };
Page page=Page::Home;
app::AutonId selected=app::AutonId::AwpSafe;
app::Snapshot state{};
lv_obj_t *body=nullptr,*title=nullptr,*footer=nullptr,*battery=nullptr,*nav=nullptr;
lv_obj_t *live_battery=nullptr,*live_imu=nullptr,*live_drive=nullptr,*live_elevator=nullptr;
lv_obj_t *diag_battery=nullptr,*diag_drive=nullptr,*diag_subsystems=nullptr;
lv_color_t color(uint32_t value){return lv_color_hex(value);}
void style(lv_obj_t* obj,uint32_t bg,uint32_t border=BG){lv_obj_set_style_bg_color(obj,color(bg),LV_PART_MAIN|LV_STATE_DEFAULT);lv_obj_set_style_bg_opa(obj,LV_OPA_COVER,LV_PART_MAIN|LV_STATE_DEFAULT);lv_obj_set_style_border_color(obj,color(border),LV_PART_MAIN|LV_STATE_DEFAULT);lv_obj_set_style_border_width(obj,border==BG?0:1,LV_PART_MAIN|LV_STATE_DEFAULT);lv_obj_set_style_radius(obj,7,LV_PART_MAIN|LV_STATE_DEFAULT);}
lv_obj_t* label(lv_obj_t* parent,const char* value,uint32_t tint=TEXT){auto* out=lv_label_create(parent);lv_label_set_text(out,value);lv_obj_set_style_text_color(out,color(tint),LV_PART_MAIN|LV_STATE_DEFAULT);return out;}
void clear(){lv_obj_clean(body);lv_obj_set_flex_flow(body,LV_FLEX_FLOW_COLUMN);lv_obj_set_style_pad_all(body,8,LV_PART_MAIN|LV_STATE_DEFAULT);lv_obj_set_style_pad_row(body,6,LV_PART_MAIN|LV_STATE_DEFAULT);}
void nav_button(lv_obj_t* parent,const char* caption,Page destination){auto* b=lv_btn_create(parent);style(b,PANEL,CYAN);lv_obj_set_height(b,30);lv_obj_set_flex_grow(b,1);lv_obj_add_event_cb(b,[](lv_event_t* e){if(lv_event_get_code(e)==LV_EVENT_CLICKED)show(static_cast<Page>(reinterpret_cast<uintptr_t>(lv_event_get_user_data(e))));},LV_EVENT_CLICKED,reinterpret_cast<void*>(static_cast<uintptr_t>(destination)));auto* l=label(b,caption,CYAN);lv_obj_center(l);}
void navigation(){if(nav){lv_obj_del(nav);nav=nullptr;}nav=lv_obj_create(axiom_active_screen());style(nav,BG);lv_obj_set_size(nav,460,34);lv_obj_align(nav,LV_ALIGN_BOTTOM_MID,0,-4);lv_obj_set_flex_flow(nav,LV_FLEX_FLOW_ROW);lv_obj_set_style_pad_column(nav,5,LV_PART_MAIN|LV_STATE_DEFAULT);lv_obj_set_style_pad_all(nav,0,LV_PART_MAIN|LV_STATE_DEFAULT);nav_button(nav,"HOME",Page::Home);nav_button(nav,"AUTON",Page::Autonomous);nav_button(nav,"DIAGNOSTICS",Page::Diagnostics);}
void heading(const char* text,const char* sub){lv_label_set_text(title,text);lv_label_set_text(footer,sub);}
void draw_home(){char bat[40],imu[32],temps[48];std::snprintf(bat,sizeof(bat),"BATTERY %.0f%%",state.battery);std::snprintf(imu,sizeof(imu),"IMU %s",state.imu_ready?"READY":"CALIBRATING");std::snprintf(temps,sizeof(temps),"DRIVE %.0f / %.0f C",state.left_temp,state.right_temp);heading(state.health,"HOME / LIVE ROBOT STATUS");clear();label(body,"AXIOM V5",CYAN);live_battery=label(body,bat,GREEN);live_imu=label(body,imu,state.imu_ready?GREEN:AMBER);live_drive=label(body,temps,TEXT);live_elevator=label(body,state.elevator_homed?"ELEVATOR HOMED":"ELEVATOR NOT HOMED",state.elevator_homed?GREEN:AMBER);}
void draw_auton(){heading("SELECT ROUTINE","AUTONOMOUS SELECTOR");clear();label(body,"TOUCH A ROUTINE TO SELECT IT",MUTED);const auto* list=auton::entries();for(std::size_t i=0;i<auton::count();++i){const auto item=list[i];auto* b=lv_btn_create(body);style(b,PANEL,item.id==selected?CYAN:LINE);lv_obj_set_size(b,440,38);lv_obj_add_event_cb(b,[](lv_event_t* e){selected=static_cast<app::AutonId>(reinterpret_cast<uintptr_t>(lv_event_get_user_data(e)));show(Page::Autonomous);},LV_EVENT_CLICKED,reinterpret_cast<void*>(static_cast<uintptr_t>(item.id)));label(b,item.name,item.id==selected?CYAN:TEXT);label(b,item.description,MUTED);}}
void draw_diag(){char line[72];heading(state.health,"DIAGNOSTICS / LIVE HARDWARE");clear();std::snprintf(line,sizeof(line),"BATTERY %.0f%%   IMU %s",state.battery,state.imu_ready?"READY":"CALIBRATING");diag_battery=label(body,line,TEXT);std::snprintf(line,sizeof(line),"DRIVE LEFT %.1f C   RIGHT %.1f C",state.left_temp,state.right_temp);diag_drive=label(body,line,TEXT);std::snprintf(line,sizeof(line),"INTAKE %.1f C   ELEVATOR %.1f C",state.intake_temp,state.elevator_temp);diag_subsystems=label(body,line,TEXT);label(body,state.elevator_homed?"ELEVATOR HOMED":"ELEVATOR NOT HOMED",state.elevator_homed?GREEN:AMBER);}
void render(){live_battery=live_imu=live_drive=live_elevator=nullptr;diag_battery=diag_drive=diag_subsystems=nullptr;if(page==Page::Home)draw_home();else if(page==Page::Autonomous)draw_auton();else draw_diag();navigation();}
}
void init(){auto* screen=axiom_active_screen();lv_obj_clean(screen);style(screen,BG);title=label(screen,"AXIOM",TEXT);lv_obj_align(title,LV_ALIGN_TOP_LEFT,12,7);battery=label(screen,"",MUTED);lv_obj_align(battery,LV_ALIGN_TOP_RIGHT,-12,10);body=lv_obj_create(screen);style(body,BG);lv_obj_set_size(body,460,165);lv_obj_align(body,LV_ALIGN_TOP_MID,0,34);footer=label(screen,"",CYAN);lv_obj_align(footer,LV_ALIGN_BOTTOM_LEFT,12,-5);render();}
void show(Page destination){page=destination;render();}
void set_snapshot(const app::Snapshot& snapshot){state=snapshot;char text[80];if(battery){std::snprintf(text,sizeof(text),"BAT %.0f%%",state.battery);lv_label_set_text(battery,text);}if(live_battery){std::snprintf(text,sizeof(text),"BATTERY %.0f%%",state.battery);lv_label_set_text(live_battery,text);}if(live_imu){std::snprintf(text,sizeof(text),"IMU %s",state.imu_ready?"READY":"CALIBRATING");lv_label_set_text(live_imu,text);}if(live_drive){std::snprintf(text,sizeof(text),"DRIVE %.0f / %.0f C",state.left_temp,state.right_temp);lv_label_set_text(live_drive,text);}if(live_elevator)lv_label_set_text(live_elevator,state.elevator_homed?"ELEVATOR HOMED":"ELEVATOR NOT HOMED");if(diag_battery){std::snprintf(text,sizeof(text),"BATTERY %.0f%%   IMU %s",state.battery,state.imu_ready?"READY":"CALIBRATING");lv_label_set_text(diag_battery,text);}if(diag_drive){std::snprintf(text,sizeof(text),"DRIVE LEFT %.1f C   RIGHT %.1f C",state.left_temp,state.right_temp);lv_label_set_text(diag_drive,text);}if(diag_subsystems){std::snprintf(text,sizeof(text),"INTAKE %.1f C   ELEVATOR %.1f C",state.intake_temp,state.elevator_temp);lv_label_set_text(diag_subsystems,text);}}
Page current_page(){return page;}
app::AutonId selected_auton(){return selected;}
}
