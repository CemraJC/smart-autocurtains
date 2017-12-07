/*

Title: Curtain Control Library
Author: Jason Cemra
Date: 07/12/2017

Description: This library handles the controlling of a stepper motor on a
pulley system with a homing limit switch. It manages the settings for the
program as well.

*/

#ifndef CURTAINCONTROL_H
#define CURTAINCONTROL_H

#include "Arduino.h"
#include <CheapStepper.h>
#include <InputControl.h>
#include <EEPROM.h>

#ifndef DEBUGGING
#define DEBUGGING true
#endif

#if DEBUGGING
#	define DBG_PRINT(...)    Serial.print(__VA_ARGS__)
#	define DBG_PRINTLN(...)  Serial.println(__VA_ARGS__)
#else
#	define DBG_PRINT(...)
#	define DBG_PRINTLN(...)
#endif

// Constants
#define SETTINGS_ADDR 0 // The starting address to write EEPROM settings to
#define MAX_SETTINGS EEPROM.length() - SETTINGS_ADDR // Total limit of setting bytes we can have

#define AUTO_OVERRIDE_TIME 60*60*1000 // 1 hour - the time before automatic features kick in again.
#define SETTING_WRITE_TIME 5*60*1000 // Time to wait before writing the settings to EEPROM.

#define OPEN_DIRECTION true // Direction to drive the stepper to open the curtains (true for clockwise, false for CCW)
							// This is also the direction that the stepper will run to trip the homing switch
#define CLOSE_DIRECTION !OPEN_DIRECTION

#define TOTAL_STEPS 4096	// Number of steps for a full revolution


// Stores the current settings
// NOTE: If adding more settings, must also add write routines for 
// it (because C++ requires wizardry to do this dynamically)
struct Settings {
	// Number of steps in the close direction to reach "away" 
	// from the "home" position
	unsigned long away = 0;

	// Feature enabled/disabled triggers
	bool autodawn = false;
	bool autotemp = false;

	long remote_open;
	long remote_close;
	long remote_cancel;
	long remote_autodawn;
	long remote_autotemp;
};

struct SettingsAddresses {
	int away = SETTINGS_ADDR;

	int remote_open = SETTINGS_ADDR + 4;
	int remote_close = SETTINGS_ADDR + 4*2;
	int remote_cancel = SETTINGS_ADDR + 4*3;
	int remote_autodawn = SETTINGS_ADDR + 4*4;
	int remote_autotemp = SETTINGS_ADDR + 4*5;

	int autodawn = SETTINGS_ADDR + 4*5 + 1;
	int autotemp = SETTINGS_ADDR + 4*5 + 2;
};


class CurtainControl {
public:
	CurtainControl(unsigned short s1, unsigned short s2, unsigned short s3, unsigned short s4) : s1(s1), s2(s2), s3(s3), s4(s4), stepper(s1, s2, s3, s4) {};

	void poll(); // Used to keep track of asynchronous functions
	void init(); // Used to setup the pin modes etc

	// settings
	void trigger_write(); // Triggers the delayed write (call this one)
	void read_settings(); // Reads settings into local memory
	Settings settings; // Modified in-place, then written manually
	const SettingsAddresses settings_addr; // Address specs

	// stepper control
	void set_home(); // Sets wherever the stepper is as "home"
	void open(); // Moves to the home position. Knows when to stop, but also expects a home signal.
	void close(); // Moves to the away position (no feedback)
	void cancel(); // Stops any current action
	void step(bool open); // Moves the stepper in the direction specified by a 90deg turn
	void get_location(); // Returns the position specifier (between 0 and 1 for home and away)
	bool is_moving(); // Returns true or false based on if the curtain is moving or not

private:
	// Pins
	unsigned short s1, s2, s3, s4;
	
	void write_settings(); // Actually writes the settings
	
	// A timestamp to facilitate waiting before writing the settings
	unsigned long settings_write_trigger = 0; 

	CheapStepper stepper;
	bool in_motion;
	void set_target(long target); // Actually writes the settings

	// A number between 0 (for "home") and 1 (for "away")
	// This number is meant to be current.
	long stepper_pos = -1; // if -1, position is unknown (needs homing)
	long stepper_target = 0; // Where the stepper should be going to.
};


#endif