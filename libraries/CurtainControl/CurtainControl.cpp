/*

Title: Curtain Control Library Implementations
Author: Jason Cemra
Date: 07/12/2017

Description: This library handles the controlling of a stepper motor on a
pulley system with a homing limit switch. It manages the settings for the
program as well.

*/

#include "CurtainControl.h"

void CurtainControl::init() {
	pinMode(s1, OUTPUT);
	pinMode(s2, OUTPUT);
	pinMode(s3, OUTPUT);
	pinMode(s4, OUTPUT);

	stepper.setRpm(16);

	// Reads settings from eeprom into local memory
	read_settings();
}

void CurtainControl::poll() {
	write_settings(); // Will only work when the trigger is set

	if (stepper_target != stepper_pos && !in_motion) {
		int steps = stepper_target - stepper_pos;

		// If steps is > 0, then our target location is in the close direction
		bool dir = (steps > 0)?(CLOSE_DIRECTION):(OPEN_DIRECTION);
		stepper.newMove(dir, abs(steps));

		stepper.run();
		in_motion = true;

		DBG_PRINT("Poll: ");
		DBG_PRINT(stepper_target);
		DBG_PRINT(" ");
		DBG_PRINT(stepper_pos);
		DBG_PRINT(" ");
		DBG_PRINTLN(stepper.getStepsLeft());

	} else if (stepper.getStepsLeft() != 0) {
		stepper.run();
	} else if (stepper.getStepsLeft() == 0) {
		stepper_pos = stepper_target;
	}

	if (stepper_pos == stepper_target) {
		stepper.stop();
		in_motion = false;
	}
}


void CurtainControl::trigger_write() {
	settings_write_trigger = millis();
}

void CurtainControl::write_settings() {
	if (settings_write_trigger != 0 && millis() - settings_write_trigger >= SETTING_WRITE_TIME) { // Timer is up
		DBG_PRINTLN("CurtainControl: Actually writing settings.");
		settings_write_trigger = 0; // stop it

		// Write all the settings to the right places
		EEPROM.put(settings_addr.away, settings.away);
		EEPROM.put(settings_addr.autodawn, settings.autodawn);
		EEPROM.put(settings_addr.autotemp, settings.autotemp);
		EEPROM.put(settings_addr.remote_open, settings.remote_open);
		EEPROM.put(settings_addr.remote_close, settings.remote_close);
		EEPROM.put(settings_addr.remote_cancel, settings.remote_cancel);
		EEPROM.put(settings_addr.remote_autodawn, settings.remote_autodawn);
		EEPROM.put(settings_addr.remote_autotemp, settings.remote_autotemp);
	}
}

void CurtainControl::read_settings() {
	// Puts the EEPROM state (may not be most recent)
	// into the local settings state
	EEPROM.get(settings_addr.away, settings.away);
	EEPROM.get(settings_addr.autodawn, settings.autodawn);
	EEPROM.get(settings_addr.autotemp, settings.autotemp);
	EEPROM.get(settings_addr.remote_open, settings.remote_open);
	EEPROM.get(settings_addr.remote_close, settings.remote_close);
	EEPROM.get(settings_addr.remote_cancel, settings.remote_cancel);
	EEPROM.get(settings_addr.remote_autodawn, settings.remote_autodawn);
	EEPROM.get(settings_addr.remote_autotemp, settings.remote_autotemp);
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


// Sets current location as home.
void CurtainControl::set_home() {
	if (stepper_pos != 0 || stepper_target != 0) {
		DBG_PRINTLN("CurtainControl: Setting home.");
		stepper_pos = 0;
		stepper_target = 0;
	} else {
		DBG_PRINTLN("CurtainControl: Already at home position.");	
	}
	stepper.stop();
}

void CurtainControl::open() {
	set_target(0);
}

void CurtainControl::close() {
	set_target(settings.away);
}

void CurtainControl::cancel() {

	// Figure out where we are like this: We take where we were, and where we
	// want to be.  Then, take away the steps to where we want to be, from
	// where we want to be. This gives us an absolute displacement from where 
	// we were. Then, find out which direction we wanted to go in, and finally

	if (in_motion) {
		DBG_PRINT("Cancel: ");
		DBG_PRINT(stepper_target);
		DBG_PRINT(" ");
		DBG_PRINT(stepper_pos);
		DBG_PRINT(" ");
		
		bool dir = (stepper_target < stepper_pos)?(OPEN_DIRECTION):(CLOSE_DIRECTION);
		stepper_pos = stepper_pos + ((dir == OPEN_DIRECTION)?(-1):(1)) * abs(abs(stepper_target - stepper_pos) - abs(stepper.getStepsLeft()));
		
		DBG_PRINT(dir == OPEN_DIRECTION);
		DBG_PRINT(" ");
		DBG_PRINT(stepper_pos);
		DBG_PRINT(" ");
		DBG_PRINTLN(stepper.getStepsLeft());

		// Stop motion
		stepper_target = stepper_pos;
	} else {
		DBG_PRINTLN("CurtainControl: Not moving, cannot cancel.");
	}


}

void CurtainControl::step(bool open) {

	if (stepper_pos == -1) {
		DBG_PRINTLN("CurtainControl: Home unknown. Cannot Move.");
		return;
	} else if (in_motion) {
		// DBG_PRINTLN("CurtainControl: Still moving, won't start new step."); // Causes trouble with motor
		return;
	}

	bool dir = open?(OPEN_DIRECTION):(CLOSE_DIRECTION);
	
	stepper_target = stepper_pos + ((dir)?(-1):(1))*(TOTAL_STEPS/4);

	if (stepper_target < 0) {\
		DBG_PRINTLN("CurtainControl: Tried to move past home (-ve)");
		stepper_target = 0;
	} else if (stepper_target > settings.away) {
		DBG_PRINTLN("CurtainControl: Tried to move past away (+ve)");
		stepper_target = settings.away;
	}
}

// Sets a new target (if not already in motion)
void CurtainControl::set_target(long target) {
	if (!in_motion && stepper_target != target) {
		DBG_PRINTLN(String("Setting target to ") + String(target));
		stepper_target = target;
	}
}

void CurtainControl::get_location() {
	return stepper_pos;
}

bool CurtainControl::is_moving() {
	return in_motion;
}
