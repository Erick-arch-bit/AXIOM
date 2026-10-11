#include "axiom/ui/ui.hpp"
#include "axiom/ui/lvgl_compat.hpp"
#include "pros/rtos.hpp"
#include <cstdint>
#include <cstdio>

namespace axiom::ui {
namespace {
constexpr uint32_t BG=0x10151C, SURFACE=0x19232E, LINE=0x2F4354;
constexpr uint32_t TEXT=0xF2F6FA, MUTED=0x8FA2B5, CYAN=0x24C8D8;
constexpr uint32_t GREEN=0x47D18C, AMBER=0xF2B84B, RED=0xF05D67;
Page page=Page::Home; app::AutonId auton=app::AutonId::AwpSafe;
app::Snapshot state{}; lv_obj_t *body=nullptr,*title=nullptr,*footer=nullptr,*nav=nullptr,*battery=nullptr;
lv_color_t c(uint32_t x){return lv_color_hex(x);}
void style(lv_obj_t* o,uint32_t bg,uint32_t border=BG){lv_obj_set_style_bg_color(o,c(bg),LV_PART_MAIN|LV_STATE_DEFAULT);lv_obj_set_style_bg_opa(o,LV_OPA_COVER,LV_PART_MAIN|LV_STATE_DEFAULT);lv_obj_set_style_border_color(o,c(border),LV_PART_MAIN|LV_STATE_DEFAULT);lv_obj_set_style_border_width(o,border==BG?0:1,LV_PART_MAIN|LV_STATE_DEFAULT);lv_obj_set_style_radius(o,7,LV_PART_MAIN|LV_STATE_DEFAULT);}
lv_obj_t* text(lv_obj_t* p,const char* s,uint32_t col=TEXT){auto* l=lv_label_create(p);lv_label_set_text(l,s);lv_obj_set_style_text_color(l,c(col),LV_PART_MAIN|LV_STATE_DEFAULT);return l;}
lv_obj_t* box(lv_obj_t* p,int w,int h){auto* o=lv_obj_create(p);style(o,SURFACE,LINE);lv_obj_set_size(o,w,h);lv_obj_set_style_pad_all(o,7,LV_PART_MAIN|LV_STATE_DEFAULT);return o;}
void clear(){lv_obj_clean(body);lv_obj_set_flex_flow(body,LV_FLEX_FLOW_COLUMN);lv_obj_set_style_pad_all(body,8,LV_PART_MAIN|LV_STATE_DEFAULT);lv_obj_set_style_pad_row(body,6,LV_PART_MAIN|LV_STATE_DEFAULT);}
void nav_button(lv_obj_t* p,const char* s,Page destination){auto* b=lv_btn_create(p);style(b,SURFACE,CYAN);lv_obj_set_height(b,30);lv_obj_set_flex_grow(b,1);lv_obj_add_event_cb(b,[](lv_event_t* e){if(lv_event_get_code(e)==LV_EVENT_CLICKED)show(static_cast<Page>(reinterpret_cast<uintptr_t>(lv_event_get_user_data(e))));},LV_EVENT_CLICKED,reinterpret_cast<void*>(static_cast<uintptr_t>(destination)));auto* l=text(b,s,CYAN);lv_obj_center(l);}
void navigation(){if(nav){lv_obj_del(nav);nav=nullptr;}nav=lv_obj_create(axiom_active_screen());style(nav,BG);lv_obj_set_size(nav,460,34);lv_obj_align(nav,LV_ALIGN_BOTTOM_MID,0,-4);lv_obj_set_flex_flow(nav,LV_FLEX_FLOW_ROW);lv_obj_set_style_pad_column(nav,5,LV_PART_MAIN|LV_STATE_DEFAULT);lv_obj_set_style_pad_all(nav,0,LV_PART_MAIN|LV_STATE_DEFAULT);nav_button(nav,"HOME",Page::Home);nav_button(nav,"AUTON",Page::Autonomous);nav_button(nav,"DIAGNOSTICS",Page::Diagnostics);}
void header(const char* a,const char* b){lv_label_set_text(title,a);lv_label_set_text(footer,b);}
void draw_home(){header("SYSTEM READY","HOME / ROBOT STATUS");clear();auto* st=box(body,440,28);lv_obj_set_flex_flow(st,LV_FLEX_FLOW_ROW);lv_obj_set_style_pad_column(st,10,LV_PART_MAIN|LV_STATE_DEFAULT);text(st,"ROBOT LARGE",CYAN);text(st,"HEALTH OK",GREEN);auto* r=lv_obj_create(body);style(r,BG);lv_obj_set_size(r,440,58);lv_obj_set_flex_flow(r,LV_FLEX_FLOW_ROW);lv_obj_set_style_pad_column(r,6,LV_PART_MAIN|LV_STATE_DEFAULT);auto* a=box(r,137,50);text(a,"BATTERY",MUTED);text(a,"94%",GREEN);auto* b=box(r,137,50);text(b,"IMU",MUTED);text(b,"OK",GREEN);auto* d=box(r,137,50);text(d,"ODOM",MUTED);text(d,"OK",CYAN);auto* r2=lv_obj_create(body);style(r2,BG);lv_obj_set_size(r2,440,58);lv_obj_set_flex_flow(r2,LV_FLEX_FLOW_ROW);lv_obj_set_style_pad_column(r2,6,LV_PART_MAIN|LV_STATE_DEFAULT);auto* p=box(r2,137,50);text(p,"POSE",MUTED);text(p,"36.2 / 18.7",TEXT);auto* t=box(r2,137,50);text(t,"DRIVE TEMP",MUTED);text(t,"36 C",AMBER);auto* q=box(r2,137,50);text(q,"POSSESSION",MUTED);text(q,"NONE",MUTED);}
void auton_button(const char* name,const char* desc,app::AutonId id){auto* b=lv_btn_create(body);style(b,SURFACE,id==auton?CYAN:LINE);lv_obj_set_size(b,440,38);lv_obj_set_flex_flow(b,LV_FLEX_FLOW_ROW);lv_obj_set_style_pad_column(b,10,LV_PART_MAIN|LV_STATE_DEFAULT);lv_obj_add_event_cb(b,[](lv_event_t* e){if(lv_event_get_code(e)==LV_EVENT_CLICKED){auton=static_cast<app::AutonId>(reinterpret_cast<uintptr_t>(lv_event_get_user_data(e)));show(Page::Autonomous);}},LV_EVENT_CLICKED,reinterpret_cast<void*>(static_cast<uintptr_t>(id)));text(b,name,id==auton?CYAN:TEXT);text(b,desc,MUTED);}
void draw_auton(){header("SELECT ROUTINE","AUTONOMOUS SELECTOR");clear();text(body,"SELECT AN AUTONOMOUS ROUTINE",MUTED);auton_button("AWP SAFE","12 PINS / 4 GOALS / MIDFIELD",app::AutonId::AwpSafe);auton_button("PINS + TOGGLE","PRIORITY: QUADRANT CONTROL",app::AutonId::PinsToggle);auton_button("SKILLS","ONE-MINUTE SKILLS PROFILE",app::AutonId::Skills);}
void draw_diag(){header(state.health,"DIAGNOSTICS / HARDWARE");clear();text(body,"LIVE HEALTH MONITOR",MUTED);auto* d=box(body,440,126);lv_obj_set_flex_flow(d,LV_FLEX_FLOW_COLUMN);text(d,"DRIVE      L 1 / 2 / 3     OK",TEXT);text(d,"          R 4 / 5 / 6     OK",TEXT);text(d,"SENSORS   IMU             OK",GREEN);text(d,"          TRACKERS        OK",GREEN);text(d,state.elevator_homed?"SUBSYSTEMS ELEVATOR       HOMED":"SUBSYSTEMS ELEVATOR       NOT HOMED",state.elevator_homed?GREEN:AMBER);}
void render(){if(page==Page::Home)draw_home();else if(page==Page::Autonomous)draw_auton();else draw_diag();navigation();}
}
void init(){auto* s=axiom_active_screen();lv_obj_clean(s);style(s,BG);title=text(s,"SYSTEM READY",TEXT);lv_obj_align(title,LV_ALIGN_TOP_LEFT,12,7);battery=text(s,"AXIOM / LARGE / BAT 94%",MUTED);lv_obj_align(battery,LV_ALIGN_TOP_RIGHT,-12,10);body=lv_obj_create(s);style(body,BG);lv_obj_set_size(body,460,165);lv_obj_align(body,LV_ALIGN_TOP_MID,0,34);footer=text(s,"HOME / ROBOT STATUS",CYAN);lv_obj_align(footer,LV_ALIGN_BOTTOM_LEFT,12,-5);render();}
void show(Page p){page=p;render();}
void set_snapshot(const app::Snapshot& s){state=s;if(battery){static char b[48];std::snprintf(b,sizeof(b),"AXIOM / LARGE / BAT %.0f%%",s.battery);lv_label_set_text(battery,b);}}
Page current_page(){return page;}
app::AutonId selected_auton(){return auton;}
}