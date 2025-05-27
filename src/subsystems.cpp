#include "main.h"  // IWYU pragma: keep

// Internal targets to aid tasks
Colors allianceColor = Colors::NEUTRAL;
Colors matchColor = allianceColor;
int intakeTarget = 0;

//
// Wrappers
//

void setIntake(int speed) {
	if(autonMode != AutonMode::BRAIN) {
		intake.move(speed);
		intakeTarget = speed;
	}
}

void setAlliance(Colors alliance) {
	allianceColor = alliance;
	matchColor = alliance;
}
void sendHaptic(string input) { controllerInput = input; }

//
// Operator Control
//

void setIntakeOp() {
	if(master.get_digital(pros::E_CONTROLLER_DIGITAL_L2))
		setIntake(127);
	else if(master.get_digital(pros::E_CONTROLLER_DIGITAL_L1))
		setIntake(-127);
	else {
		setIntake(0);
	}
}

//
// Color sort
//

void colorToggle() {
	if(allianceColor == matchColor)
		allianceColor = Colors::NEUTRAL;
	else
		allianceColor = matchColor;
}

void colorSet(Colors color) {
	// Set on screen elements to the corresponding color
	lv_color32_t color_use = theme_accent;
	if(color == Colors::RED)
		color_use = red;
	else if(color == Colors::BLUE)
		color_use = blue;
	lv_obj_set_style_bg_color(colorind, color_use, LV_PART_MAIN);
}

Colors colorGet() {
	double hue = 0;
	if(colorSens.get_proximity() > 100) {
		hue = colorSens.get_hue();
		if((hue > 340 && hue < 360) || (hue > 0 && hue < 20))
			return Colors::RED;
		else if(hue > 210 && hue < 240)
			return Colors::BLUE;
	}
	return Colors::NEUTRAL;
}

bool colorCompare(Colors color) {
	if((int)allianceColor < 2 && (int)color < 2) return allianceColor != color;
	return false;
}

void colorTask() {
	Colors color;
	int sortTime = 0;
	colorSens.set_integration_time(10);
	colorSens.set_led_pwm(100);
	while(true) {
		color = colorGet();
		colorSet(color);
		if(!pros::competition::is_disabled()) {
			// COLOR SORT ALG
		}
		pros::delay(10);
	}
}

//
// Other tasks
//

void controllerTask() {
	string pattern = "";
	int timer = 0;
	float tempDrive;
	float tempIntake;
	while(true) {
		// Update timer and rumble controller
		if(!pros::competition::is_autonomous() && !pros::competition::is_disabled()) {
			if(pattern == "") {
				if(timer == 475)
					pattern = "-";
				else if(timer == 375)
					pattern = "--";
				else if((timer >= 350 && timer < 375) || (timer >= 500 && timer < 525))
					pattern = ".";
				else
					pattern = controllerInput;
			}
			if(timer % 5 == 0 || controllerInput != "") {
				master.rumble(pattern.c_str());
				controllerInput = "";
				pattern = "";
			}
			timer++;
		}
		pros::delay(50);

		// Update temperature variables and print to controller
		tempDrive = (chassis.left_motors[0].get_temperature() + chassis.left_motors[1].get_temperature() + chassis.left_motors[2].get_temperature() +
					 chassis.right_motors[0].get_temperature() + chassis.right_motors[1].get_temperature() + chassis.right_motors[2].get_temperature()) /
					6;
		tempIntake = intake.get_temperature();

		if(tempDrive <= 30)
			pros::c::controller_print(pros::E_CONTROLLER_MASTER, 0, 0, "drive: cool, %.0f°C", tempDrive);
		else if(tempDrive > 30 && tempDrive <= 50)
			pros::c::controller_print(pros::E_CONTROLLER_MASTER, 0, 0, "drive: warm, %.0f°C", tempDrive);
		else if(tempDrive > 50)
			pros::c::controller_print(pros::E_CONTROLLER_MASTER, 0, 0, "drive: hot, %.0f°C", tempDrive);
		pros::delay(50);

		if(tempIntake <= 30)
			pros::c::controller_print(pros::E_CONTROLLER_MASTER, 1, 0, "intke: cool, %.0f°C", tempIntake);
		else if(tempIntake > 30 && tempIntake <= 50)
			pros::c::controller_print(pros::E_CONTROLLER_MASTER, 1, 0, "intke: warm, %.0f°C", tempIntake);
		else if(tempIntake > 50)
			pros::c::controller_print(pros::E_CONTROLLER_MASTER, 1, 0, "intke: hot, %.0f°C", tempIntake);
		pros::delay(50);
	}
}

/*
void unjamTask() {
	int jamtime = 0;
	while(true) {
		if(intake.get_temperature() < 50) {
				if(!jamState && !jamDisabled && intakeTarget != 0 && abs(intake.get_actual_velocity()) <= 20) {
					jamtime++;
					if(jamtime > 20) {
						jamtime = 0;
						jamState = true;
					}
				}

				if(jamState) {
					intake.move(-intakeTarget);
					jamtime++;
					if(jamtime > 20) {
						jamtime = 0;
						jamState = false;
						setIntake(intakeTarget);
					}
				}
			}
		pros::delay(10);
	}
}
*/