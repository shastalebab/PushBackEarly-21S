#include "main.h"
#include "drive.hpp"

// Chassis constructor
ez::Drive chassis(
	// These are your drive motors, the first motor is used for sensing!
	{-1, -19, -20},  // Left Chassis Ports (negative port will reverse it!)
	{8, 9, 10},	   // Right Chassis Ports (negative port will reverse it!)

	12,				// IMU Port
	WHEEL_DIAMETER,	// Wheel Diameter (Remember, 4" wheels without screw holes are actually 4.125!)
	450);			// Wheel RPM = cartridge * (motor gear / wheel gear)

void initialize() {
	// Print our branding over your terminal :D
	ez::ez_template_print();

	pros::delay(500);  // Stop the user from doing anything while legacy ports configure

	// Configure your chassis controls
	chassis.opcontrol_curve_buttons_toggle(false);	// Enables modifying the controller curve with buttons on the joysticks
	chassis.opcontrol_drive_activebrake_set(2.0);	// Sets the active brake kP. We recommend ~2.  0 will disable.
	chassis.opcontrol_curve_default_set(
		3.0, 0.0);	// Defaults for curve. If using tank, only the first parameter is used. (Comment this line out if you have an SD card!)

	// Set the drive to your own constants from autons.cpp!
	default_constants();

	// Autonomous Selector using LLEMU
	auton_sel.selector_populate({
		{right_split, "right_split", "right side 4 + 5", gray},
		{right_awp, "right_awp", "right side 3 + 1 + 5 solo AWP", violet},
		{left_split, "left_split", "left side 4 + 5", lv_color_lighten(gray, 125)},
		{left_awp, "left_awp", "left side 1 + 5 + 3 solo AWP", pink}
	});

	// Initialize chassis and auton selector
	chassis.initialize();
	uiInit();
	pros::Task ColorTask(colorTask);
	pros::Task AntiJamTask(antiJamTask);
	pros::Task ControllerTask(controllerTask);
	pros::Task PathViewerTask(pathViewerTask);
	pros::Task AngleCheckTask(angleCheckTask);
	pros::Task MotorUpdateTask(motorUpdateTask);
	master.rumble(chassis.drive_imu_calibrated() ? "." : "---");
}

void disabled() {
	// . . .
}

void competition_initialize() { 
	autonMode = BRAIN;
 }

void autonomous() {
	chassis.pid_targets_reset();				// Resets PID targets to 0
	chassis.drive_imu_reset();					// Reset gyro position to 0
	chassis.drive_sensor_reset();				// Reset drive sensors to 0
	chassis.odom_xyt_set(0_in, 0_in, 0_deg);	// Set the current position, you can start at a specific position with this
	chassis.drive_brake_set(MOTOR_BRAKE_HOLD);	// Set motors to hold.  This helps autonomous consistency

	autonMode = PLAIN;
	autonPath = {};
	auton_sel.selector_callback();	// Calls selected auton from autonomous selector
}

void opcontrol() {
	chassis.drive_brake_set(pros::E_MOTOR_BRAKE_BRAKE);
	autonMode = PLAIN;

	while(true) {
		if(!probing) chassis.opcontrol_tank();  // Tank control

		setIntakeOp();
		setScraperOp();

		pros::delay(ez::util::DELAY_TIME);	// This is used for timer calculations!  Keep this ez::util::DELAY_TIME
	}
}
