#include "main.h"  // IWYU pragma: keep

// Internal targets to aid tasks
Colors allianceColor = NEUTRAL;
Colors matchColor = allianceColor;
int firstTarget = 0;
int sorterTarget = 0;
int hoarderTarget = 0;
int indexerTarget = 0;
bool inputLock = false;
bool jamDelay = false;

// Complex motors
Jammable first = Jammable(&intakeFirst, &firstTarget, 20, 50, 60, true, false);
Jammable sorter = Jammable(&intakeSorter, &sorterTarget, 20, 50, 50, false, false);
Jammable hoarder = Jammable(&intakeHoarder, &hoarderTarget, 20, 20, 50, true, false);
Jammable indexer = Jammable(&intakeIndexer, &indexerTarget, 20, 20, 100, true, false);

//
// Wrappers
//

void setIntake(int first_speed, int second_speed, int third_speed, int fourth_speed) {
	if(autonMode != BRAIN) {
		if(first.lock != true) {
			first.motor->move(first_speed);
		}
		if(sorter.lock != true) {
			sorter.motor->move(second_speed);
		}
		if(hoarder.lock != true) {
			hoarder.motor->move(third_speed);
		}
		if(indexer.lock != true) {
			indexer.motor->move(fourth_speed);
		}
		firstTarget = first_speed;
		sorterTarget = second_speed;
		hoarderTarget = third_speed;
		indexerTarget = fourth_speed;
	}
}

void setIntake(int intake_speed, int snail_speed, int outtake_speed) { setIntake(intake_speed, snail_speed, snail_speed, outtake_speed); }

void setIntake(int intake_speed, int outtake_speed) { setIntake(intake_speed, intake_speed, intake_speed, outtake_speed); }

void setIntake(int speed) { setIntake(speed, speed, speed, speed); }

void setScraper(bool state) {
	if(autonMode != BRAIN) {
		scraper.set(state);
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

bool shift() { return master.get_digital(pros::E_CONTROLLER_DIGITAL_R2); }

void setIntakeOp() {
	if(master.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_L1) || master.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_R1) ||
	   master.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_L2))
		jamDelay = true;
	if(shift()) {
		if(master.get_digital(pros::E_CONTROLLER_DIGITAL_L1))  // sorting
			setIntake(127, 0, 0);
		else if(master.get_digital(pros::E_CONTROLLER_DIGITAL_L2))	// low goal evil scoring
			setIntake(-127, -30, 127, -127);
		else if(master.get_digital(pros::E_CONTROLLER_DIGITAL_R1))	// mid goal scoring
			setIntake(60, -127, 127, -8);
		else {
			setIntake(0);
			first.lock = false;
			sorter.lock = false;
			hoarder.lock = false;
			indexer.lock = false;
		}
	} else if(!inputLock) {
		if(master.get_digital(pros::E_CONTROLLER_DIGITAL_L1)) {	 // stowing
			setIntake(127);
		} else if(master.get_digital(pros::E_CONTROLLER_DIGITAL_L2)) {	// low goal safe scoring
			setIntake(-50, -50, 90, 0);
			first.limit = 5;
		} else if(master.get_digital(pros::E_CONTROLLER_DIGITAL_R1)) {	// top goal scoring scoring
			setIntake(127, -127, 127, 127);
		} else {
			setIntake(0);

			first.limit = 20;
			first.lock = false;
			sorter.lock = false;
			hoarder.lock = false;
			indexer.lock = false;
		}
	}

	if(master.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_R2)) sendHaptic(".");
}

void setScraperOp() { scraper.button_toggle(master.get_digital(pros::E_CONTROLLER_DIGITAL_Y)); }

//
// Color sort
//

void colorToggle() {
	if(allianceColor == matchColor)
		allianceColor = NEUTRAL;
	else
		allianceColor = matchColor;
}

void colorSet(Colors color, lv_obj_t* object) {
	// Set on screen elements to the corresponding color
	lv_color32_t color_use = theme_accent;
	if(color == RED)
		color_use = red;
	else if(color == BLUE)
		color_use = blue;
	lv_obj_set_style_bg_color(object, color_use, LV_PART_MAIN);
}

Colors colorGet() {
	double hue = 0;
	if(proximitySens.get_proximity() > 100) {
		hue = colorSens.get_hue();
		if((hue > 340 && hue < 360) || (hue > 0 && hue < 20))
			return RED;
		else if(hue > 210 && hue < 240)
			return BLUE;
	}
	return NEUTRAL;
}

bool colorCompare(Colors color) {
	if((int)allianceColor < 2 && (int)color < 2) return allianceColor != color;
	return false;
}

void colorTask() {
	Colors color;
	int sortTime = 0;
	bool sleep = false;
	colorSens.set_integration_time(10);
	proximitySens.set_integration_time(10);
	colorSens.set_led_pwm(100);
	proximitySens.set_led_pwm(100);
	while(true) {
		color = colorGet();
		colorSet(color, colorInd);
		if(!pros::competition::is_disabled()) {
			if(colorCompare(color) && !sleep) {
				if(sortTime < 10) {
					inputLock = true;
					intakeSorter.move(-(util::sgn(sorterTarget)) * 10);
					sortTime++;
					pros::delay(100);
				} else {
					sleep = true;
					intakeSorter.move(sorterTarget);
				}
			} else {
				sortTime = 0;
				sleep = false;
				inputLock = false;
			}
		}
		pros::delay(10);
	}
}

//
// Anti-jam
//

void Jammable::checkJam() {
	if(this->motor->get_temperature() > this->maxTemp) {
		// cout << "Temperature too high: " << this->motor->get_temperature() << " °C\n";
		return;
	}

	if(this->ignoreSort == false && inputLock == true) {
		// cout << "Input locked\n";
		return;
	}

	// cout << "target: " << abs(*(this->target)) << ", " << "actual: " << this->motor->get_actual_velocity() << " limit: " << this->limit << "\n";

	if(abs(*(this->target)) > 0 && abs(this->motor->get_actual_velocity()) < this->limit) {
		this->clock++;
		cout << this->clock << "\n";
		if(this->clock > this->attempts) {
			if(this->pause) {
				this->lock = true;
				this->motor->move(0);
				// cout << "Paused\n";
			} else {
				this->lock = true;
				this->motor->move(-(*(this->target)));
				pros::delay(100);
				this->motor->move(*(this->target));
				this->lock = false;
				// cout << "Unjammed\n";
			}
			this->clock = 0;
		}
	} else {
		this->clock = 0;
		// cout << "Jam resolved\n";
	}
}

void antiJamTask() {
	while(true) {
		if(jamDelay) {
			pros::delay(200);
			jamDelay = false;
		}
		first.checkJam();
		sorter.checkJam();
		hoarder.checkJam();
		indexer.checkJam();
		// cout << "====================\n";
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
					pattern = "";  // "- -"
				else if(timer >= 500 && timer < 525)
					pattern = "";  // "."
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
		tempIntake = (intakeFirst.get_temperature() + intakeSorter.get_temperature() + intakeIndexer.get_temperature()) / 3;

		if(tempDrive <= 30)
			pros::c::controller_print(pros::E_CONTROLLER_MASTER, 0, 0, "drive: cool, %.0f°C     ", tempDrive);
		else if(tempDrive > 30 && tempDrive <= 50)
			pros::c::controller_print(pros::E_CONTROLLER_MASTER, 0, 0, "drive: warm, %.0f°C     ", tempDrive);
		else if(tempDrive > 50)
			pros::c::controller_print(pros::E_CONTROLLER_MASTER, 0, 0, "drive: hot, %.0f°C     ", tempDrive);
		pros::delay(50);

		if(tempIntake <= 30)
			pros::c::controller_print(pros::E_CONTROLLER_MASTER, 1, 0, "intke: cool, %.0f°C     ", tempIntake);
		else if(tempIntake > 30 && tempIntake <= 50)
			pros::c::controller_print(pros::E_CONTROLLER_MASTER, 1, 0, "intke: warm, %.0f°C     ", tempIntake);
		else if(tempIntake > 50)
			pros::c::controller_print(pros::E_CONTROLLER_MASTER, 1, 0, "intke: hot, %.0f°C     ", tempIntake);
		pros::delay(50);

		// Print selected auton to controller
		if(!(!pros::competition::is_autonomous() && !pros::competition::is_disabled()))
			pros::c::controller_print(pros::E_CONTROLLER_MASTER, 2, 0, (auton_sel.selector_name + "        ").c_str());
		pros::delay(50);
	}
}