#pragma once

#include "EZ-Template/api.hpp"
#include "api.h"
#include "pros/motors.hpp"

extern Drive chassis;

// Your motors, sensors, etc. should go here.  Below are examples

inline pros::Optical colorSens(13);
inline pros::Optical proximitySens(1);

inline pros::Motor intakeNone(21);
inline pros::Motor intakeFirst(3);
inline pros::Motor intakeSorter(-5);
inline pros::Motor intakeHoarder(6);
inline pros::Motor intakeIndexer(7);
inline ez::Piston scraper('A');
inline ez::Piston dumper('B');

class Jammable {
   private:
	int clock = 0;

   public:
	pros::Motor* motor;
	int* target;
	int limit;
	int attempts;
	float maxTemp;
	bool ignoreSort;
	bool pause;

	bool lock;
	void checkJam();

	Jammable() {
		motor = &intakeFirst;
		target = nullptr;
		attempts = 20;
		limit = 4;
		maxTemp = 55;
		ignoreSort = true;
		pause = false;
		lock = false;
	}
	Jammable(pros::Motor* Motor, int* Target, int Limit, int Attempts, float MaxTemp, bool IgnoreSort, bool Pause) {
		motor = Motor;
		target = Target;
		attempts = Attempts;
		limit = Limit;
		maxTemp = MaxTemp;
		ignoreSort = IgnoreSort;
		pause = Pause;
		lock = false;
	}
};

enum Colors { BLUE = 0, NEUTRAL = 1, RED = 2 };

extern Colors allianceColor;
extern Jammable none;
extern Jammable first;
extern Jammable sorter;
extern Jammable hoarder;
extern Jammable indexer;

extern Jammable* targetMotor;

bool shift();

void setIntake(int first_speed, int second_speed, int third_speed, int fourth_speed);
void setIntake(int intake_speed, int snail_speed, int outtake_speed);
void setIntake(int intake_speed, int outtake_speed);
void setIntake(int speed);
void setScraper(bool state);
void setDumper(bool state);

void setAlliance(Colors alliance);
void colorToggle();
void colorSet(Colors color, lv_obj_t* object);

void sendHaptic(string input);

void setIntakeOp();
void setScraperOp();
void setDumperOp();

void colorTask();
void antiJamTask();
void controllerTask();