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
	DBG_PRINTLN(settings.away);
	DBG_PRINTLN(settings.autodawn);
	DBG_PRINTLN(settings.autotemp);
	DBG_PRINTLN(settings.remote_open, HEX);
	DBG_PRINTLN(settings.remote_close, HEX);
	DBG_PRINTLN(settings.remote_cancel, HEX);
	DBG_PRINTLN(settings.remote_autodawn, HEX);
	DBG_PRINTLN(settings.remote_autotemp, HEX);
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
	DBG_PRINTLN("CurtainControl: Triggered settings write at time:");
	settings_write_trigger = millis();
	DBG_PRINTLN(settings_write_trigger);
}

void CurtainControl::write_settings() {
	if (settings_write_trigger != 0 && millis() - settings_write_trigger >= SETTING_WRITE_TIME) { // Timer is up
		DBG_PRINTLN("CurtainControl: Actually writing settings at time from trigger:");
		DBG_PRINTLN(settings_write_trigger);
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
		EEPROM.put(settings_addr.data_indicator, SETTINGS_ID);
	}
}

void CurtainControl::read_settings() {
	if (EEPROM.read(settings_addr.data_indicator) == SETTINGS_ID) {
		DBG_PRINTLN("CurtainControl: Settings read.");
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
	} else {
		DBG_PRINTLN("CurtainControl: No EEPROM settings to read.");
	}
}

// This takes some time to do.
void CurtainControl::reset_settings() {
	for (int i = SETTINGS_ADDR; i < SETTINGS_END_ADDR; i++) {
		EEPROM.write(i, 0);
	}
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

// Blindly rotate with the expectation that we will be made to stop.
void CurtainControl::blind_rotate(bool open) {
	long steps = ((open)?(-1):(1)) * TOTAL_STEPS * MAX_BLIND_ROTATIONS;
	set_target(steps);
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
	} else if (settings.away != 0 && stepper_target > settings.away) {
		// Note: if away is zero, then we are unbounded in this direction.
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

long CurtainControl::get_location() {
	return stepper_pos;
}

bool CurtainControl::is_moving() {
	return in_motion;
}
