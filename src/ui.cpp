#include "ui.hpp"
#include "liblvgl/core/lv_obj_pos.h"
#include "liblvgl/core/lv_obj_style.h"
#include "liblvgl/lv_api_map.h"
#include "liblvgl/widgets/lv_img.h"
#include "main.h"  // IWYU pragma: keep

// // // // // // Tasks & Non-UI // // // // // //

//
// Object creation
//

//lv_obj_t* main_tv = lv_tileview_create(NULL);

//lv_obj_t* autoSelector = lv_tileview_add_tile(main_tv, 0, 1, LV_DIR_NONE);
lv_obj_t* autoSelector = lv_obj_create(NULL);
lv_obj_t* colorind = lv_obj_create(autoSelector);
lv_obj_t* coloroverlay = lv_img_create(autoSelector);
//lv_obj_t* pageUp[2] = {lv_label_create(main_tv), lv_label_create(main_tv)};
//lv_obj_t* pageDown[2] = {lv_label_create(main_tv), lv_label_create(main_tv)};

lv_obj_t* autonTable = lv_list_create(autoSelector);
lv_obj_t* autonUp = lv_label_create(autoSelector);
lv_obj_t* autonDown = lv_label_create(autoSelector);
lv_obj_t* autonDesc = lv_label_create(autoSelector);
lv_obj_t* autonField = lv_img_create(autoSelector);
lv_obj_t* autonRobot = lv_img_create(autonField);
lv_obj_t* angleViewer;
lv_obj_t* angleText;

lv_style_t pushback;

LV_IMG_DECLARE(red_alliance);
LV_IMG_DECLARE(blue_alliance);
LV_IMG_DECLARE(colorindOverlay);
LV_IMG_DECLARE(robot);

// // // // // // Tasks & Non-UI // // // // // //

//
// Auton Selector
//

string controllerInput = "";
bool aligning = false;

AutonSel auton_sel;

void AutonSel::selector_populate(vector<AutonObj> auton_list) { autons.insert(autons.end(), auton_list.begin(), auton_list.end()); }

void angleCheckTask() {
	while(true) {
		if(aligning) {
			auto target = autonPath.size() > 0 ? autonPath[0].t : 0;
			auto current = fmod(chassis.drive_imu_get(), 360);
			if(current < 0) current += 360;
			lv_label_set_text(angleText,
							  (util::to_string_with_precision(current, 2) + " °" + "\ntarget: " + util::to_string_with_precision(target, 2)).c_str());
			if(target + 0.15 >= current && target - 0.15 <= current)
				lv_obj_set_style_bg_color(angleViewer, green, LV_PART_MAIN);
			else
				lv_obj_set_style_bg_color(angleViewer, red, LV_PART_MAIN);
		}
		pros::delay(10);
	}
}

int pathIter = 0;
vector<Coordinate> pathDisplay;

void resetViewer(bool full) {
	if(full) {
		auto preference = autonMode;
		autonMode = AutonMode::BRAIN;
		autonPath = {};
		auton_sel.selector_callback();
		pathDisplay = smoothPath(injectPath(autonPath, 1), 1, 4);
		autonMode = preference;
		lv_img_set_src(autonField, &(allianceColor == Colors::BLUE ? blue_alliance : red_alliance));
	}
	pathIter = 0;
}

void pathViewerTask() {
	while(true) {
		if(pathIter < pathDisplay.size() && pathDisplay.size() > 1) {
			lv_obj_clear_flag(autonRobot, LV_OBJ_FLAG_HIDDEN);
			lv_obj_set_pos(autonRobot, (2 * pathDisplay[pathIter].x) - 11, 130 - (2 * pathDisplay[pathIter].y));
			if(pathIter < pathDisplay.size() - 1) {
				lv_img_set_angle(autonRobot, 10 * (pathDisplay[pathIter].t));
				pros::delay(33 - 2.5 * sqrt(pathDisplay[pathIter].main));
			}
			if(pathIter == 1) pros::delay(500);
			pathIter++;
		} else if(pathIter >= pathDisplay.size()) {
			pros::delay(1000);
			resetViewer(false);
		}
		pros::delay(10);
	}
}

// // // // // // UI // // // // // //

//
// Main UI
//

void uiInit() {
	// Initialize style
	lv_style_init(&pushback);
	lv_style_set_bg_color(&pushback, theme_color);
	lv_style_set_outline_color(&pushback, theme_accent);
	lv_style_set_text_color(&pushback, theme_accent);
	lv_style_set_bg_opa(&pushback, 255);
	lv_style_set_outline_width(&pushback, 3);
	lv_style_set_border_width(&pushback, 0);
	lv_style_set_text_font(&pushback, &lv_font_montserrat_16);
	lv_style_set_radius(&pushback, 0);

	// Set up flags
	//lv_obj_clear_flag(main_tv, LV_OBJ_FLAG_SCROLLABLE);
	lv_obj_clear_flag(colorind, LV_OBJ_FLAG_SCROLLABLE);
	//lv_obj_add_flag(pageUp[0], LV_OBJ_FLAG_CLICKABLE);
	//lv_obj_add_flag(pageDown[1], LV_OBJ_FLAG_CLICKABLE);
	//lv_obj_add_flag(pageUp[0], LV_OBJ_FLAG_CLICKABLE);
	//lv_obj_add_flag(pageDown[1], LV_OBJ_FLAG_CLICKABLE);

	// Add styles
	//lv_obj_add_style(main_tv, &pushback, LV_PART_MAIN);
	lv_obj_add_style(colorind, &pushback, LV_PART_MAIN);
	//lv_obj_set_style_bg_opa(main_tv, 255, LV_PART_MAIN);
	//lv_obj_set_style_outline_opa(main_tv, 0, LV_PART_MAIN);

	// Initialize screens
	autoSelectorInit();

	// Set up ring indicator
	lv_img_set_src(coloroverlay, &colorindOverlay);
	lv_obj_set_parent(colorind, autoSelector);
	lv_obj_set_size(colorind, 51, 51);
	lv_obj_set_pos(colorind, 421, 20);
	lv_obj_set_pos(coloroverlay, 421, 20);
	lv_obj_set_style_outline_width(colorind, 0, LV_PART_MAIN);
	lv_obj_set_style_outline_width(coloroverlay, 0, LV_PART_MAIN);
	lv_obj_move_foreground(colorind);
	lv_obj_move_foreground(coloroverlay);

	// Load main screen
	lv_scr_load(autoSelector);
	//lv_obj_set_tile(main_tv, autoSelector, LV_ANIM_OFF);
}

//
// Auton Selector UI
//

static void selectAuton(lv_event_t* e) {
	AutonObj* getAuton = (AutonObj*)lv_event_get_user_data(e);
	lv_obj_t* target = lv_event_get_target(e);
	for(int i = 0; i < lv_obj_get_child_cnt(autonTable); i++) {
		lv_obj_t* auton = lv_obj_get_child(autonTable, i);
		lv_obj_clear_state(auton, LV_STATE_CHECKED);
	}
	lv_obj_add_state(target, LV_STATE_CHECKED);
	lv_label_set_text(autonDesc, ((*getAuton).desc).c_str());
	lv_obj_set_style_bg_color(autonDesc, lv_color_darken((*getAuton).color, 150), LV_PART_MAIN);
	auton_sel.selector_callback = (*getAuton).callback;
	resetViewer(true);
}

static void autonUpEvent(lv_event_t* e) { lv_obj_scroll_by_bounded(autonTable, 0, lv_obj_get_height(autonTable), LV_ANIM_ON); }

static void autonDownEvent(lv_event_t* e) { lv_obj_scroll_by_bounded(autonTable, 0, -lv_obj_get_height(autonTable), LV_ANIM_ON); }

static void angleCheckCloseEvent(lv_event_t* e) { aligning = false; }

lv_event_cb_t AngleCheckCloseEvent = angleCheckCloseEvent;

static void angleCheckEvent(lv_event_t* e) {
	angleViewer = lv_msgbox_create(NULL, "check alignment", "0°", NULL, true);
	angleText = lv_msgbox_get_text(angleViewer);
	aligning = true;

	lv_obj_add_event_cb(lv_msgbox_get_close_btn(angleViewer), AngleCheckCloseEvent, LV_EVENT_PRESSED, NULL);
	lv_obj_add_style(lv_msgbox_get_close_btn(angleViewer), &pushback, LV_PART_MAIN);
	lv_obj_add_style(angleViewer, &pushback, LV_PART_MAIN);
	lv_obj_set_style_text_font(angleViewer, &lv_font_montserrat_30, LV_PART_MAIN);
	lv_obj_set_style_text_font(lv_msgbox_get_title(angleViewer), &lv_font_montserrat_14, LV_PART_MAIN);
	lv_obj_set_style_text_font(lv_msgbox_get_close_btn(angleViewer), &lv_font_montserrat_24, LV_PART_MAIN);
	lv_obj_set_width(angleViewer, 300);
	lv_obj_align(angleViewer, LV_ALIGN_CENTER, 0, 0);
}

lv_event_cb_t SelectAuton = selectAuton;
lv_event_cb_t AutonUpEvent = autonUpEvent;
lv_event_cb_t AutonDownEvent = autonDownEvent;
lv_event_cb_t AngleCheckEvent = angleCheckEvent;

void autoSelectorInit() {
	// Add base styles
	lv_obj_add_style(autonTable, &pushback, LV_PART_MAIN);
	lv_obj_add_style(autonTable, &pushback, LV_PART_ITEMS);
	lv_obj_add_style(autonDesc, &pushback, LV_PART_MAIN);
	lv_obj_add_style(autonField, &pushback, LV_PART_MAIN);
	lv_obj_add_style(autonRobot, &pushback, LV_PART_MAIN);
	lv_obj_add_style(autonUp, &pushback, LV_PART_MAIN);
	lv_obj_add_style(autonDown, &pushback, LV_PART_MAIN);

	// Set image sources and default text
	lv_img_set_src(autonField, &red_alliance);
	lv_img_set_src(autonRobot, &robot);
	lv_label_set_text(autonDesc, "No auton selected");
	lv_label_set_text(autonUp, LV_SYMBOL_UP "\n" LV_SYMBOL_UP "\n" LV_SYMBOL_UP "\n" LV_SYMBOL_UP "\n" LV_SYMBOL_UP);
	lv_label_set_text(autonDown, LV_SYMBOL_DOWN "\n" LV_SYMBOL_DOWN "\n" LV_SYMBOL_DOWN "\n" LV_SYMBOL_DOWN "\n" LV_SYMBOL_DOWN);

	// Set flags
	lv_obj_add_flag(autonUp, LV_OBJ_FLAG_CLICKABLE);
	lv_obj_add_flag(autonDown, LV_OBJ_FLAG_CLICKABLE);
	lv_obj_add_flag(autonField, LV_OBJ_FLAG_CLICKABLE);
	lv_obj_add_flag(autonRobot, LV_OBJ_FLAG_HIDDEN);

	// Set sizes of objects
	lv_obj_set_size(autonTable, 160, 206);
	lv_obj_set_size(autonDesc, 288, 57);
	lv_obj_set_size(autonField, 288, 144);

	// Align and set positions of objects
	lv_obj_align(autonTable, LV_ALIGN_LEFT_MID, 5, 0);
	lv_obj_align(autonDesc, LV_ALIGN_TOP_RIGHT, -5, 17);
	lv_obj_align(autonField, LV_ALIGN_BOTTOM_RIGHT, -5, -17);
	lv_obj_align(autonUp, LV_ALIGN_CENTER, -64, -68);
	lv_obj_align(autonDown, LV_ALIGN_CENTER, -64, 68);

	// Modify styles

	lv_obj_set_style_text_opa(autonUp, 128, LV_STATE_PRESSED);
	lv_obj_set_style_text_opa(autonDown, 128, LV_STATE_PRESSED);
	lv_obj_set_style_bg_opa(autonRobot, 0, LV_PART_MAIN);

	lv_obj_set_style_outline_width(autonTable, 1, LV_PART_ITEMS);
	lv_obj_set_style_outline_width(autonField, 5, LV_STATE_PRESSED);
	lv_obj_set_style_outline_width(autonRobot, 0, LV_PART_MAIN);
	lv_obj_set_style_outline_width(autonUp, 0, LV_PART_MAIN);
	lv_obj_set_style_outline_width(autonDown, 0, LV_PART_MAIN);

	lv_obj_set_style_text_font(autonUp, &lv_font_montserrat_16, LV_PART_MAIN);
	lv_obj_set_style_text_font(autonDown, &lv_font_montserrat_16, LV_PART_MAIN);
	lv_obj_set_style_text_line_space(autonUp, -12, LV_PART_MAIN);
	lv_obj_set_style_text_line_space(autonDown, -12, LV_PART_MAIN);
	lv_obj_set_style_pad_all(autonDesc, 5, LV_PART_MAIN);
	lv_obj_set_style_pad_right(autonDesc, 85, LV_PART_MAIN);
	lv_obj_set_style_pad_hor(autonTable, 0, LV_PART_MAIN);

	lv_obj_set_scrollbar_mode(autonTable, LV_SCROLLBAR_MODE_OFF);

	// Add events
	lv_obj_add_event_cb(autonUp, AutonUpEvent, LV_EVENT_CLICKED, NULL);
	lv_obj_add_event_cb(autonDown, AutonDownEvent, LV_EVENT_CLICKED, NULL);
	lv_obj_add_event_cb(autonField, AngleCheckEvent, LV_EVENT_CLICKED, NULL);

	// Set up list
	for(int i = 0; i < auton_sel.autons.size(); i++) {
		lv_obj_t* new_auto = lv_list_add_btn(autonTable, NULL, (auton_sel.autons[i].name).c_str());
		lv_obj_add_style(new_auto, &pushback, LV_PART_MAIN);
		lv_obj_set_style_text_font(new_auto, &pros_font_dejavu_mono_18, LV_PART_MAIN);
		lv_obj_set_style_bg_color(new_auto, lv_color_darken(auton_sel.autons[i].color, 150), LV_PART_MAIN);
		lv_obj_set_style_outline_width(new_auto, 1, LV_PART_MAIN);
		lv_obj_set_style_outline_width(new_auto, 4, LV_STATE_CHECKED);
		lv_obj_set_style_outline_width(new_auto, 6, LV_STATE_PRESSED);
		lv_obj_set_style_bg_opa(new_auto, 120, LV_STATE_CHECKED);
		lv_obj_set_style_bg_opa(new_auto, 60, LV_STATE_PRESSED);
		lv_obj_set_style_pad_hor(new_auto, 8, LV_PART_MAIN);
		lv_obj_add_event_cb(new_auto, SelectAuton, LV_EVENT_CLICKED, &auton_sel.autons[i]);
	}
}