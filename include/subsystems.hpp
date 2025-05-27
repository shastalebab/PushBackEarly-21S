#pragma once

#include "EZ-Template/api.hpp"
#include "api.h"

extern Drive chassis;

// Your motors, sensors, etc. should go here.  Below are examples

inline pros::Optical colorSens(11);

inline pros::Motor intake(7);

enum class Colors {
    RED = 0,
    BLUE = 1,
    NEUTRAL = 2
};

extern Colors allianceColor;
extern Colors matchColor;

void setIntake(int speed);

void setAlliance(Colors alliance);
void colorToggle();

void sendHaptic(string input);

void setIntakeOp();

void colorTask();
void controllerTask();
void unjamTask();