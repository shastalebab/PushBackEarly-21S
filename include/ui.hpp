#pragma once

#include "EZ-Template/api.hpp"
#include "api.h"
#include "autons.hpp"

const lv_color32_t theme_color = lv_color_hex(0x00060a);
const lv_color32_t theme_accent = lv_color_hex(0xe3f4ff);
const lv_color32_t red = lv_color_hex(0xff3643);
const lv_color32_t blue = lv_color_hex(0x01b1f0);
const lv_color32_t green = lv_color_hex(0x22d428);
const lv_color32_t violet = lv_color_hex(0xb672f2);
const lv_color32_t pink = lv_color_hex(0xffade7);
const lv_color32_t gray = lv_color_hex(0x575757);

// Auton selector
void angleCheckTask();
void pathViewerTask();

class AutonObj {
    public:
        function<void()> callback = move_forward;
        string name = "no name";
        string desc = "no description";
        lv_color32_t color = theme_color;
};

class AutonSel {
    public:
        vector<AutonObj> autons = {};
        string selector_name = "";
        function<void()> selector_callback = move_forward;
        void selector_populate(vector<AutonObj> auton_list);
};

extern string controllerInput;
extern AutonSel auton_sel;

// Main UI
extern lv_obj_t* colorInd;
extern lv_obj_t* autoSelector;

void uiInit();

// Auton Selector UI
void autoSelectorInit();