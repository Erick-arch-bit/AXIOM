#pragma once
#include "liblvgl/lvgl.h"
#include "pros/apix.h"
inline lv_obj_t* axiom_active_screen() {
    #if defined(LVGL_VERSION_MAJOR) && LVGL_VERSION_MAJOR >= 9
        return lv_screen_active();
    #else
        return lv_scr_act();
    #endif
}
