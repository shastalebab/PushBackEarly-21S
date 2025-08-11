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
	chassis.pid_drive_constants_forward_set(13.0, 0.0, 61.25);		   // Fwd constants, used for odom and non odom motions
	chassis.pid_drive_constants_backward_set(25.25, 0.0, 147.5);		   // Rev constants, used for odom and non odom motions
	chassis.pid_heading_constants_set(11.75, 0.0, 31.25);		   // Holds the robot straight while going forward without odom
	chassis.pid_turn_constants_set(3.0, 0.10, 22.75, 30.0);	   // Turn in place constants
	chassis.pid_swing_constants_set(9.25, 0.0, 72.5);		   // Swing constants
	chassis.pid_odom_angular_constants_set(6.5, 0.0, 52.5);	   // Angular control for odom motions
	chassis.pid_odom_boomerang_constants_set(5.8, 0.0, 32.5);  // Angular control for boomerang motions
	chassis.pid_drive_constants_get();

	// Exit conditions
	chassis.pid_turn_exit_condition_set(90_ms, 3_deg, 250_ms, 7_deg, 500_ms, 500_ms, false);
	chassis.pid_swing_exit_condition_set(70_ms, 3_deg, 250_ms, 7_deg, 500_ms, 500_ms, false);
	chassis.pid_drive_exit_condition_set(70_ms, 1.7_in, 180_ms, 4_in, 200_ms, 200_ms, false);
	chassis.pid_odom_turn_exit_condition_set(40_ms, 3_deg, 170_ms, 6_deg, 500_ms, 750_ms, false);
	chassis.pid_odom_drive_exit_condition_set(90_ms, 1_in, 250_ms, 3_in, 500_ms, 750_ms, false);
	chassis.pid_turn_chain_constant_set(4_deg);
	chassis.pid_swing_chain_constant_set(5_deg);
	chassis.pid_drive_chain_constant_set(4_in);
	chassis.drive_imu_scaler_set(1.00005);

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
	chassis.pid_swing_set(LEFT_SWING, degrees, SWING_SPEED, raw);
	chassis.pid_wait();
}

void heading_test(int degrees) {
	chassis.pid_drive_set(12, 127);
	chassis.pid_wait_quick_chain();
	chassis.pid_turn_set(degrees, 127, raw);
	chassis.pid_wait_quick_chain();
	chassis.pid_drive_set(12, 127);
}

void constants_test() {
	setPosition(85.44, 20.86, 45);
	driveSet(12, 127);
	pidWait(WAIT);
	turnSet(90, 127);
	pidWait(WAIT);
	turnSet(0, 127);
	pidWait(WAIT);
	turnSet(45, 127);
	pidWait(WAIT);
}

//
// RIGHT AUTONS
//

void right_split() {
	setPosition(85.44, 20.86, 45);
	// Collect and score middle three blocks in mid goal
	driveSet(29.75, DRIVE_SPEED);
	pidWait(WAIT);
	swingSet(LEFT_SWING, -45, 90, -45, ccw);
	pidWait(WAIT);
	setIntake(127);
	driveSet(23.75, DRIVE_SPEED);
	pidWait(WAIT);
	setScraper(true);
	delayMillis(200);
	setIntake(-80, -80, -80, 0);
	delayMillis(1000);
	// Grab blocks under long goal
	setIntake(127);
	swingSet(RIGHT_SWING, 61, SWING_SPEED);
	pidWait(WAIT);
	setScraper(false);
	driveSet(28, DRIVE_SPEED);
	pidWait(WAIT);
	setScraper(true);
	delayMillis(400);
	// Align to loader/long goal
	driveSet(-28, DRIVE_SPEED, true);
	pidWait(WAIT);
	moveToPoint({122, 23}, fwd, TURN_SPEED);
	pidWait(WAIT);
	turnSet(180, TURN_SPEED);
	pidWait(WAIT);
	// Grab blocks from loader and score on long goal
	driveSet(17, 60);
	delayMillis(800);
	driveSet(-8, DRIVE_SPEED);
	pidWait(WAIT);
	turnSet(0, TURN_SPEED);
	setScraper(false);
	pidWait(WAIT);
	driveSet(20, 60);
	pidWait(WAIT);
	setIntake(127, -127, 127, 127);
}

void right_greed() {
	setPosition(85.44, 20.86, 45);
	// Collect and score middle three blocks in mid goal
	driveSet(29.75, DRIVE_SPEED, true);
	pidWait(WAIT);
	delayMillis(200);
	swingSet(LEFT_SWING, -45, 90, -45, ccw);
	pidWait(WAIT);
	driveSet(19, DRIVE_SPEED, true);
	delayMillis(200);
	setIntake(127);
	pidWait(WAIT);
	// Grab blocks under long goal
	setIntake(127);
	turnSet(61, TURN_SPEED);
	pidWait(WAIT);
	driveSet(22, DRIVE_SPEED);
	pidWait(WAIT);
	setScraper(true);
	delayMillis(400);
	// Align to loader/long goal
	driveSet(-22, DRIVE_SPEED, true);
	pidWait(WAIT);
	moveToPoint({122, 23}, fwd, TURN_SPEED);
	pidWait(WAIT);
	delayMillis(200);
	turnSet(180, TURN_SPEED);
	pidWait(WAIT);
	// Grab blocks from loader and score on long goal
	driveSet(17, 60);
	delayMillis(800);
	driveSet(-8, DRIVE_SPEED);
	pidWait(WAIT);
	delayMillis(1000);
	turnSet(0, TURN_SPEED);
	setScraper(false);
	pidWait(WAIT);
	driveSet(20, 60);
	pidWait(WAIT);
	setIntake(127, -127, 127, 127);
}

void right_awp() {
	setPosition(85.44, 20.86, 45);
	// Collect and score middle three blocks in mid goal
	driveSet(29.75, DRIVE_SPEED);
	pidWait(WAIT);
	swingSet(LEFT_SWING, -45, 90, -45, ccw);
	pidWait(WAIT);
	setIntake(127);
	driveSet(23.75, DRIVE_SPEED);
	pidWait(WAIT);
	setScraper(true);
	delayMillis(200);
	setIntake(-80, -80, -80, 0);
	delayMillis(1000);
	// Grab blocks on other side of field and score
	setIntake(127);
	driveSet(-7.75, DRIVE_SPEED, true);
	pidWait(WAIT);
	setScraper(false);
	turnSet(-90, TURN_SPEED);
	pidWait(WAIT);
	driveSet(43, DRIVE_SPEED);
	pidWaitUntil(30_in);
	chassis.pid_speed_max_set(80);
	pidWait(WAIT);
	setIntake(127, 40, 0, 0);
	turnSet(45, TURN_SPEED);
	pidWait(WAIT);
	driveSet(12, DRIVE_SPEED);
	setIntake(70, 80, 50, -25);
	pidWait(WAIT);
	delayMillis(1500);
	// Align to loader
	setIntake(127);
	driveSet(-47, DRIVE_SPEED, true);
	pidWait(WAIT);
	turnSet(180, TURN_SPEED);
	setScraper(true);
	pidWait(WAIT);
	// Grab blocks from loader and score on long goal
	driveSet(14, 80);
	delayMillis(900);
	driveSet(-8, DRIVE_SPEED);
	pidWait(CHAIN);
	turnSet(0, TURN_SPEED);
	setScraper(false);
	pidWait(WAIT);
	driveSet(12.5, DRIVE_SPEED);
	pidWait(WAIT);
	setIntake(127, -127, 127, 127);
}

//
// LEFT AUTONS
//

void left_split() {
	setPosition(58.56, 20.86, -45);
	// Collect and score middle three blocks in mid goal
	driveSet(30.5, DRIVE_SPEED, true);
	pidWait(WAIT);
	delayMillis(200);
	swingSet(RIGHT_SWING, 45, 90, -45, cw);
	pidWait(WAIT);
	driveSet(27.75, DRIVE_SPEED, true);
	delayMillis(200);
	setIntake(70, 80, 50, -25);
	pidWait(WAIT);
	delayMillis(1500);
	// Grab blocks under long goal
	setIntake(127);
	swingSet(LEFT_SWING, -61, SWING_SPEED);
	pidWait(WAIT);
	driveSet(28, DRIVE_SPEED);
	pidWait(WAIT);
	setScraper(true);
	delayMillis(400);
	// Align to loader/long goal
	driveSet(-28, DRIVE_SPEED, true);
	pidWait(WAIT);
	moveToPoint({24, 23}, fwd, TURN_SPEED);
	pidWait(WAIT);
	turnSet(180, TURN_SPEED);
	pidWait(WAIT);
	// Grab blocks from loader and score on long goal
	driveSet(17, 60);
	delayMillis(800);
	driveSet(-8, DRIVE_SPEED);
	pidWait(WAIT);
	turnSet(0, TURN_SPEED);
	setScraper(false);
	pidWait(WAIT);
	driveSet(20, 60);
	pidWait(WAIT);
	setIntake(127, -127, 127, 127);
}

void left_greed() {
	setPosition(58.56, 20.86, -45);
	// Collect and score middle three blocks in mid goal
	driveSet(30.5, DRIVE_SPEED, true);
	pidWait(WAIT);
	delayMillis(200);
	swingSet(RIGHT_SWING, 45, 90, -45, cw);
	pidWait(WAIT);
	driveSet(19, DRIVE_SPEED, true);
	delayMillis(200);
	setIntake(127);
	pidWait(WAIT);
	// Grab blocks under long goal
	setIntake(127);
	turnSet(-61, TURN_SPEED);
	pidWait(WAIT);
	driveSet(22, DRIVE_SPEED);
	pidWait(WAIT);
	setScraper(true);
	delayMillis(400);
	// Align to loader/long goal
	driveSet(-22, DRIVE_SPEED, true);
	pidWait(WAIT);
	moveToPoint({22, 23}, fwd, TURN_SPEED);
	pidWait(WAIT);
	delayMillis(200);
	turnSet(180, TURN_SPEED);
	pidWait(WAIT);
	// Grab blocks from loader and score on long goal
	driveSet(17, 60);
	delayMillis(800);
	driveSet(-8, DRIVE_SPEED);
	pidWait(WAIT);
	delayMillis(1000);
	turnSet(0, TURN_SPEED);
	setScraper(false);
	pidWait(WAIT);
	driveSet(20, 60);
	pidWait(WAIT);
	setIntake(127, -127, 127, 127);
}

void left_awp() {
	setPosition(58.56, 20.86, -45);
	// Collect and score middle three blocks in mid goal
	driveSet(29.75, DRIVE_SPEED);
	pidWait(WAIT);
	swingSet(RIGHT_SWING, 45, 90, -45, cw);
	pidWait(WAIT);
	setIntake(127);
	driveSet(28.75, DRIVE_SPEED);
	delayMillis(200);
	setIntake(70, 80, 50, -25);
	pidWait(WAIT);
	delayMillis(1500);
	// Grab blocks on other side of field and score
	setIntake(127);
	driveSet(-12.75, DRIVE_SPEED, true);
	pidWait(WAIT);
	turnSet(90, TURN_SPEED);
	pidWait(WAIT);
	driveSet(45, DRIVE_SPEED);
	pidWaitUntil(30_in);
	chassis.pid_speed_max_set(80);
	pidWait(WAIT);
	setIntake(127, 40, 0, 0);
	turnSet(-45, TURN_SPEED);
	pidWait(WAIT);
	driveSet(9, DRIVE_SPEED);
	setScraper(true);
	pidWait(WAIT);
	setIntake(-80, -80, -80, 0);
	delayMillis(1000);
	// Align to loader
	setIntake(127);
	driveSet(-42, DRIVE_SPEED, true);
	pidWait(WAIT);
	turnSet(180, TURN_SPEED);
	pidWait(WAIT);
	// Grab blocks from loader and score on long goal
	driveSet(14, 80);
	delayMillis(900);
	driveSet(-8, DRIVE_SPEED);
	pidWait(CHAIN);
	turnSet(0, TURN_SPEED);
	setScraper(false);
	pidWait(WAIT);
	driveSet(12.5, DRIVE_SPEED);
	pidWait(WAIT);
	setIntake(127, -127, 127, 127);
}

//
// SKILLS
//

void skills() {}