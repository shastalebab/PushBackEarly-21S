#include "EZ-Template/util.hpp"
#include "drive.hpp"
#include "main.h"  // IWYU pragma: keep
#include "subsystems.hpp"

/////
// For installation, upgrading, documentations, and tutorials, check out our website!
// https://ez-robotics.github.io/EZ-Template/
/////

// These are out of 127
const int DRIVE_SPEED = 110;
const int TURN_SPEED = 90;
const int SWING_SPEED = 90;

///
// Constants
///
void default_constants() {
	// P, I, D, and Start I
	chassis.pid_drive_constants_set(0.0, 0.0, 0.0);		   // Fwd/rev constants, used for odom and non odom motions
	chassis.pid_heading_constants_set(11.0, 0.0, 25.0);		   // Holds the robot straight while going forward without odom
	chassis.pid_turn_constants_set(4.25, 2.15, 47.25, 15.0);	   // Turn in place constants
	chassis.pid_swing_constants_set(6.0, 0.0, 65.0);		   // Swing constants
	chassis.pid_odom_angular_constants_set(6.5, 0.0, 52.5);	   // Angular control for odom motions
	chassis.pid_odom_boomerang_constants_set(5.8, 0.0, 32.5);  // Angular control for boomerang motions
	chassis.pid_drive_constants_get();

	// Exit conditions
	chassis.pid_turn_exit_condition_set(90_ms, 3_deg, 250_ms, 7_deg, 500_ms, 500_ms);
	chassis.pid_swing_exit_condition_set(70_ms, 3_deg, 250_ms, 7_deg, 500_ms, 500_ms);
	chassis.pid_drive_exit_condition_set(70_ms, 2.2_in, 160_ms, 6_in, 200_ms, 200_ms);
	chassis.pid_odom_turn_exit_condition_set(40_ms, 3_deg, 170_ms, 6_deg, 500_ms, 750_ms);
	chassis.pid_odom_drive_exit_condition_set(90_ms, 1_in, 250_ms, 3_in, 500_ms, 750_ms);
	chassis.pid_turn_chain_constant_set(4_deg);
	chassis.pid_swing_chain_constant_set(5_deg);
	chassis.pid_drive_chain_constant_set(4_in);

	// Slew constants
	chassis.slew_turn_constants_set(3_deg, 70);
	chassis.slew_drive_constants_set(3_in, 70);
	chassis.slew_swing_constants_set(3_in, 80);

	// The amount that turns are prioritized over driving in odom motions
	// - if you have tracking wheels, you can run this higher.  1.0 is the max
	chassis.odom_turn_bias_set(0.9);

	chassis.odom_look_ahead_set(7_in);			 // This is how far ahead in the path the robot looks at
	chassis.odom_boomerang_distance_set(16_in);	 // This sets the maximum distance away from target that the carrot point can be
	chassis.odom_boomerang_dlead_set(0.625);	 // This handles how aggressive the end of boomerang motions are

	chassis.pid_angle_behavior_set(shortest);  // Changes the default behavior for turning, this defaults it to the shortest path there
}

//
// TUNING
//

void drive_test(int inches) {
	chassis.pid_drive_set(inches, 127);
	chassis.pid_wait();
}

void turn_test(int degrees) {
	chassis.pid_turn_set(degrees, 127, raw);
	chassis.pid_wait();
}

void swing_test(int degrees) {
	chassis.pid_swing_set(LEFT_SWING, degrees, 127, 30, raw);
	chassis.pid_wait();
}

void heading_test(int degrees) {
	chassis.pid_drive_set(12, 127);
	chassis.pid_wait_quick_chain();
	chassis.pid_turn_set(degrees, 127, raw);
	chassis.pid_wait_quick_chain();
	chassis.pid_drive_set(12, 127);
}

//
// RIGHT AUTONS
//

void right_split() {
	setPosition(85.44, 20.86, 45);
	// Collect and score middle three blocks in low goal
	driveSet(30, DRIVE_SPEED);
	pidWait(WAIT);
	swingSet(LEFT_SWING, -45, 50, -20, ccw);
	pidWait(WAIT);
	setIntake(127, -5);
	driveSet(30, 40);
	pidWait(WAIT);
	delayMillis(100);
	setIntake(-90, -127, 0);
	delayMillis(1000);
	// Grab blocks under long goal
	setIntake(0);
	swingSet(RIGHT_SWING, 60, SWING_SPEED);
	pidWait(CHAIN);
	driveSet(29.5, 90);
	setIntake(127, -5);
	pidWait(WAIT);
	setScraper(true);
	delayMillis(500);
	// Align to loader/long goal
	swingSet(LEFT_SWING, 45, SWING_SPEED, 55, ccw);
	pidWait(CHAIN);
	swingSet(LEFT_SWING, 232, SWING_SPEED, 35, ccw);
	pidWait(CHAIN);
	turnSet(180, TURN_SPEED);
	pidWait(WAIT);
	// Grab blocks from loader and score on long goal
	driveSet(21, 80);
	delayMillis(1500);
	driveSet(-8, DRIVE_SPEED);
	pidWait(CHAIN);
	turnSet(0, TURN_SPEED);
	pidWait(WAIT);
	setScraper(false);
	driveSet(12.5, DRIVE_SPEED);
	pidWait(WAIT);
	setIntake(127, -127);
}

void right_greed() {}

void right_awp() {
	setPosition(83.44, 18.86, 45);
	// Collect and score middle three blocks in low goal
	driveSet(29.5, DRIVE_SPEED);
	pidWait(WAIT);
	turnSet(-45, TURN_SPEED);
	pidWait(WAIT);
	setIntake(127, 0);
	driveSet(26, 80);
	pidWait(WAIT);
	setIntake(-127, 0);
	delayMillis(1000);
	// Score preload on high goal
	swingSet(RIGHT_SWING, 135, SWING_SPEED);
	pidWait(CHAIN);
	driveSet(-10, 90);
	setIntake(127);
	pidWait(WAIT);
	delayMillis(1000);
	// Grab blocks under long goal
	swingSet(RIGHT_SWING, 45, SWING_SPEED, 55, ccw);
	pidWait(CHAIN);
	setIntake(127, 0);
	driveSet(10, DRIVE_SPEED);
	pidWait(WAIT);
	swingSet(LEFT_SWING, 294, SWING_SPEED, 50, ccw);
	pidWait(CHAIN);
	turnSet(180, TURN_SPEED);
	// Grab blocks from loader and score on long goal
	driveSet(18, DRIVE_SPEED);
	pidWait(WAIT);
	delayMillis(500);
	driveSet(-29.5, DRIVE_SPEED);
	pidWait(WAIT);
	setIntake(127);
}

//
// LEFT AUTONS
//

void left_split() {
	setPosition(58.56, 20.86, -45);
	// Collect and score middle three blocks in low goal
	driveSet(30, DRIVE_SPEED);
	pidWait(WAIT);
	swingSet(RIGHT_SWING, 45, 50, -20, cw);
	pidWait(WAIT);
	setIntake(127, -5);
	driveSet(30, 40);
	pidWait(WAIT);
	setIntake(127, 127);
	delayMillis(1500);
	// Grab blocks under long goal
	setIntake(0);
	swingSet(LEFT_SWING, -60, SWING_SPEED);
	pidWait(CHAIN);
	driveSet(29.5, 90);
	setIntake(127, -5);
	pidWait(WAIT);
	setScraper(true);
	delayMillis(500);
	// Align to loader/long goal
	swingSet(RIGHT_SWING, -45, SWING_SPEED, 55, cw);
	pidWait(CHAIN);
	swingSet(RIGHT_SWING, -232, SWING_SPEED, 35, cw);
	pidWait(CHAIN);
	turnSet(180, TURN_SPEED);
	pidWait(WAIT);
	// Grab blocks from loader and score on long goal
	driveSet(21, 80);
	delayMillis(1500);
	driveSet(-8, DRIVE_SPEED);
	pidWait(CHAIN);
	turnSet(0, TURN_SPEED);
	pidWait(WAIT);
	setScraper(false);
	driveSet(12.5, DRIVE_SPEED);
	pidWait(WAIT);
	setIntake(127, -127);
}

void left_greed() {}

void left_awp() {
	setPosition(60.56, 18.86, -45);
	// Approach bottom goal
	driveSet(16, DRIVE_SPEED);
	pidWait(WAIT);
	turnSet(45, TURN_SPEED);
	pidWait(WAIT);
	driveSet(36, DRIVE_SPEED);
	pidWait(WAIT);
	// Score preload on bottom goal (placed on top of side rollers, wedged in first stage)
	setIntake(-127);
	delayMillis(250);
	setIntake(127, 0);
	// Grab 5 blocks
	driveSet(-11, 80);
	pidWait(WAIT);
	turnSet(-90, TURN_SPEED);
	pidWait(CHAIN);
	swingSet(ez::LEFT_SWING, -45, SWING_SPEED, 70, cw);
	pidWait(WAIT);
	delayMillis(750);
	// Score 5 blocks on top goal
	swingSet(ez::LEFT_SWING, -90, SWING_SPEED, 59.7, ccw);
	pidWait(WAIT);
	turnSet(225, TURN_SPEED);
	pidWait(WAIT);
	driveSet(-5.5, 80);
	pidWait(WAIT);
	setIntake(127);
	delayMillis(1200);
	// Grab blocks from loader and score on long goal
	driveSet(47, DRIVE_SPEED);
	pidWait(WAIT);
	turnSet(180, TURN_SPEED);
	pidWait(WAIT);
	driveSet(14, DRIVE_SPEED);
	pidWait(WAIT);
	setIntake(127, 0);
	delayMillis(750);
	driveSet(-29, DRIVE_SPEED);
	pidWait(WAIT);
	setIntake(127);
}

//
// SKILLS
//

void skills() {}