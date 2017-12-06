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
#define TOTAL_STEPS 4096	// Number of steps for a full revolution


// Stores the current settings
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

const struct SettingsAddresses {
	unsigned int away = SETTINGS_ADDR;

	unsigned int remote_open = SETTINGS_ADDR + 4;
	unsigned int remote_close = SETTINGS_ADDR + 4*2;
	unsigned int remote_cancel = SETTINGS_ADDR + 4*3;
	unsigned int remote_autodawn = SETTINGS_ADDR + 4*4;
	unsigned int remote_autotemp = SETTINGS_ADDR + 4*5;

	unsigned int autodawn = SETTINGS_ADDR + 4*5 + 1;
	unsigned int autotemp = SETTINGS_ADDR + 4*5 + 2;
}

enum StepperAction {
	NONE,
	OPEN,
	CLOSE,
	HOME
}

void EEPROMWritelong(int address, long value) {
	//Decomposition from a long to 4 bytes by using bitshift.
	//One = Most significant -> Four = Least significant byte
	byte four = (value & 0xFF);
	byte three = ((value >> 8) & 0xFF);
	byte two = ((value >> 16) & 0xFF);
	byte one = ((value >> 24) & 0xFF);

	//Write the 4 bytes into the eeprom memory.
	EEPROM.write(address, four);
	EEPROM.write(address + 1, three);
	EEPROM.write(address + 2, two);
	EEPROM.write(address + 3, one);
}

long EEPROMReadlong(long address) {
	//Read the 4 bytes from the eeprom memory.
	long four = EEPROM.read(address);
	long three = EEPROM.read(address + 1);
	long two = EEPROM.read(address + 2);
	long one = EEPROM.read(address + 3);

	//Return the recomposed long by using bitshift.
	return ((four << 0) & 0xFF) + ((three << 8) & 0xFFFF) + ((two << 16) & 0xFFFFFF) + ((one << 24) & 0xFFFFFFFF);
}


// Stores the current state (position of stepper and distance to go)
struct state {
	// A number between 0 (for "home") and 1 (for "away")
	// This number is meant to be current.
	float stepper_pos = -1; // if -1, position is unknown (needs homing)
	unsigned float stepper_target = 0; // Where the stepper should be going to.

	// True if any automatic actions should pause (for the auto-override duration)
	// bool auto_override = false;

	StepperAction current_action; // Keeps track of what we were doing.

	// A timestamp to facilitate waiting before writing the settings
	unsigned long settings_write_trigger = 0; 
};


class CurtainControl {
public:
	CurtainControl(unsigned short s1, unsigned short s2, unsigned short s3, unsigned short s4) : s1(s1), s2(s2), s3(s3), s4(s4), stepper(s1, s2, s3, s4) {};

	void poll(); // Used to keep track of asynchronous functions

	// settings
	void write_settings(); // Triggers the delayed write
	Settings settings; // Modified in-place, then written manually
	SettingsAddresses settings_addr; // Address specs

	// stepper control
	void home(); // Moves to home position - does not stop until it receives a home signal.
	void open(); // Moves to the home position. Knows when to stop, but also expects a home signal.
	void close(); // Moves to the away position (no feedback)
	void cancel(); // Stops any current action
	void step(bool open); // Moves the stepper in the direction specified by a 90deg turn
	void get_location(); // Returns the position specifier (between 0 and 1 for home and away)

private:
	// Pins
	unsigned short s1, s2, s3, s4;

	CheapStepper stepper;
}


#endif