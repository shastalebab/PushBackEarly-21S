#pragma once

#include "EZ-Template/api.hpp"
#include "api.h"
#include "pros/motor_group.hpp"

extern Drive chassis;

// Your motors, sensors, etc. should go here.  Below are examples

inline pros::Optical colorSens(11);

inline pros::Motor intakeFirst(-6);
inline pros::Motor intakeSorter(5);
inline pros::Motor intakeIndexer(4);

enum Colors {
    BLUE = 0,
    NEUTRAL = 1,
    RED = 2
};

extern Colors allianceColor;
extern Colors matchColor;

bool shift();

void setIntake(int first_speed, int second_speed, int third_speed);
void setIntake(int intake_speed, int outtake_speed);
void setIntake(int speed);

void setAlliance(Colors alliance);
void colorToggle();
void colorSet(Colors color, lv_obj_t * object);

void sendHaptic(string input);

void setIntakeOp();

void colorTask();
void controllerTask();