#include "main.h"  // IWYU pragma: keep

// // // // // // Tasks & Non-UI // // // // // //

//
// Object creation
//

lv_obj_t* autoSelector = lv_obj_create(NULL);
lv_obj_t* colorInd = lv_obj_create(autoSelector);
lv_obj_t* colorOverlay = lv_img_create(autoSelector);
lv_obj_t* allianceInd = lv_obj_create(autoSelector);
lv_obj_t* allianceOverlay = lv_img_create(autoSelector);

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
bool playing = true;

AutonSel auton_sel;

void AutonSel::selector_populate(vector<AutonObj> auton_list) { autons.insert(autons.end(), auton_list.begin(), auton_list.end()); }

void angleCheckTask() {
	while(true) {
		if(aligning) {
			auto target = autonPath.size() > 0 ? autonPath[0].t : 0;
			auto current = fmod(chassis.odom_theta_get(), 360);
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
		pathDisplay = injectPath(autonPath, 1);
		autonMode = preference;
		lv_img_set_src(autonField, &(matchColor == Colors::BLUE ? blue_alliance : red_alliance));
	}
	pathIter = 0;
}

void pathViewerTask() {
	while(true) {
		if(pathIter < pathDisplay.size() && pathDisplay.size() > 1 && playing) {
			lv_obj_clear_flag(autonRobot, LV_OBJ_FLAG_HIDDEN);
			lv_obj_set_pos(autonRobot, (2 * pathDisplay[pathIter].x) - 11, 130 - (2 * pathDisplay[pathIter].y));
			if(pathIter < pathDisplay.size() - 1) {
				lv_img_set_angle(autonRobot, 10 * (pathDisplay[pathIter].t));
				if(pathDisplay[pathIter].left == KEY)
					pros::delay(pathDisplay[pathIter].right);
				else {
					double velocity = getVelocity(pathDisplay[pathIter].left) + getVelocity(pathDisplay[pathIter].right) / 2;
					if(velocity == 0) velocity = getVelocity(pathDisplay[pathIter].left);
					pros::delay(1000 * abs(getTimeToPoint(1, velocity)));
				}
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
	lv_obj_clear_flag(colorInd, LV_OBJ_FLAG_SCROLLABLE);
	lv_obj_clear_flag(allianceInd, LV_OBJ_FLAG_SCROLLABLE);

	// Add styles
	lv_obj_add_style(autoSelector, &pushback, LV_PART_MAIN);
	lv_obj_add_style(colorInd, &pushback, LV_PART_MAIN);
	lv_obj_add_style(allianceInd, &pushback, LV_PART_MAIN);
	lv_obj_set_style_bg_opa(autoSelector, 255, LV_PART_MAIN);
	lv_obj_set_style_outline_opa(autoSelector, 0, LV_PART_MAIN);

	// Initialize screens
	autoSelectorInit();

	// Set up ring indicator
	lv_img_set_src(colorOverlay, &colorindOverlay);
	lv_img_set_src(allianceOverlay, &colorindOverlay);
	lv_obj_set_size(colorInd, 51, 51);
	lv_obj_set_size(allianceInd, 51, 51);
	lv_obj_set_pos(colorInd, 421, 20);
	lv_obj_set_pos(colorOverlay, 421, 20);
	lv_obj_set_pos(allianceInd, 368, 20);
	lv_obj_set_pos(allianceOverlay, 368, 20);
	lv_obj_set_style_outline_width(colorInd, 0, LV_PART_MAIN);
	lv_obj_set_style_outline_width(colorOverlay, 0, LV_PART_MAIN);
	lv_obj_set_style_outline_width(allianceInd, 0, LV_PART_MAIN);
	lv_obj_set_style_outline_width(allianceOverlay, 0, LV_PART_MAIN);
	lv_obj_set_style_img_recolor_opa(colorOverlay, 255, LV_PART_MAIN);
	lv_obj_set_style_img_recolor_opa(allianceOverlay, 255, LV_PART_MAIN);
	lv_obj_move_foreground(colorInd);
	lv_obj_move_foreground(allianceInd);
	lv_obj_move_foreground(colorOverlay);
	lv_obj_move_foreground(allianceOverlay);
	colorSet(matchColor, allianceInd);

	// Load main screen
	lv_scr_load(autoSelector);
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
	lv_obj_set_style_bg_color(autonDesc, lv_color_darken((*getAuton).color, 80), LV_PART_MAIN);
	lv_obj_set_style_img_recolor(colorOverlay, lv_color_darken((*getAuton).color, 80), LV_PART_MAIN);
	lv_obj_set_style_img_recolor(allianceOverlay, lv_color_darken((*getAuton).color, 80), LV_PART_MAIN);
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

static void pauseEvent(lv_event_t* e) {
	auto event = lv_event_get_code(e);
	if(event == LV_EVENT_PRESSING) playing = false;
	if(event == LV_EVENT_CLICKED) playing = true;
}

static void colorEvent(lv_event_t* e) {
	matchColor = (Colors)(((int)matchColor + 1) % 3);
	colorSet(matchColor, allianceInd);
	resetViewer(true);
}

lv_event_cb_t SelectAuton = selectAuton;
lv_event_cb_t AutonUpEvent = autonUpEvent;
lv_event_cb_t AutonDownEvent = autonDownEvent;
lv_event_cb_t AngleCheckEvent = angleCheckEvent;
lv_event_cb_t PauseEvent = pauseEvent;
lv_event_cb_t ColorEvent = colorEvent;

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
	lv_obj_add_flag(allianceInd, LV_OBJ_FLAG_CLICKABLE);
	lv_obj_add_flag(allianceOverlay, LV_OBJ_FLAG_CLICKABLE);
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
	lv_obj_add_event_cb(autonField, AngleCheckEvent, LV_EVENT_SHORT_CLICKED, NULL);
	lv_obj_add_event_cb(autonField, PauseEvent, LV_EVENT_CLICKED, NULL);
	lv_obj_add_event_cb(autonField, PauseEvent, LV_EVENT_PRESSING, NULL);
	lv_obj_add_event_cb(allianceInd, colorEvent, LV_EVENT_CLICKED, NULL);
	lv_obj_add_event_cb(allianceOverlay, colorEvent, LV_EVENT_CLICKED, NULL);

	// Set up list
	for(int i = 0; i < auton_sel.autons.size(); i++) {
		lv_obj_t* new_auto = lv_list_add_btn(autonTable, NULL, (auton_sel.autons[i].name).c_str());
		lv_obj_add_style(new_auto, &pushback, LV_PART_MAIN);
		lv_obj_set_style_text_font(new_auto, &pros_font_dejavu_mono_18, LV_PART_MAIN);
		lv_obj_set_style_bg_color(new_auto, lv_color_darken(auton_sel.autons[i].color, 80), LV_PART_MAIN);
		lv_obj_set_style_outline_width(new_auto, 1, LV_PART_MAIN);
		lv_obj_set_style_outline_width(new_auto, 4, LV_STATE_CHECKED);
		lv_obj_set_style_outline_width(new_auto, 6, LV_STATE_PRESSED);
		lv_obj_set_style_bg_opa(new_auto, 220, LV_STATE_CHECKED);
		lv_obj_set_style_bg_opa(new_auto, 180, LV_STATE_PRESSED);
		lv_obj_set_style_pad_hor(new_auto, 8, LV_PART_MAIN);
		lv_obj_add_event_cb(new_auto, SelectAuton, LV_EVENT_CLICKED, &auton_sel.autons[i]);
	}
}