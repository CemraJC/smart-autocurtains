/*

Title: InputControl Library Definitions
Author: Jason Cemra
Date: 05/12/2017

*/

#include "InputControl.h"

/*
====================
SENSOR INPUT CONTROL
====================
*/

void SensorInputControl::init() {
	pinMode(light_pin, INPUT);
	pinMode(temp_pin, INPUT);
}

void SensorInputControl::poll() {
	// // Don't poll too often 
	// if (millis() - last_poll_time < MIN_POLL_DELAY) {
	// 	return;
	// }

	int light = get_light_reading();

	if (abs(last_light - light) >= LIGHT_DEBOUNCE_GAP) {
		// Check current phase
		if (light >= LIGHT_THRESHOLD) {
			last_brightness = LIGHT;
		} else if (light <= DARK_THRESHOLD) {
			last_brightness = DARK;
		}

		// Check for edges
		if (last_real_brightness != last_brightness) {
			// We have detected a first change, so trigger a wait timer
			if (phase_wait_trigger == 0) {
				phase_wait_trigger = millis();
			}
			
			if (millis() - phase_wait_trigger >= DAY_PHASE_DELAY) {
				last_real_brightness = last_brightness;
				last_phase_edge_time = millis();
				phase_wait_trigger = 0;
			}
		} else {
			// There has been no change, so stop the timer
			phase_wait_trigger = 0;
		}
	}

	// last_poll_time = millis();
}

unsigned int SensorInputControl::get_light_reading() {
	// NOTE: Dividing by 2, because using a 10k pulldown resistor
	return analogRead(light_pin)/2;
}

Brightness SensorInputControl::get_brightness() {
	poll();
	return last_brightness;
}

unsigned int SensorInputControl::get_temperature() {
	poll();
	return analogRead(temp_pin);
}


unsigned long SensorInputControl::last_light_transition() {
	return last_phase_edge_time;
}


bool SensorInputControl::is_dark() {
	poll();
	return last_real_brightness == DARK;
}

bool SensorInputControl::is_light() {
	poll();
	return last_real_brightness == LIGHT;
}

/*
==================
USER INPUT CONTROL
==================
*/

/* GLOBAL */

void UserInputControl::init() {
	pinMode(open_pin, INPUT);
	pinMode(close_pin, INPUT);
	pinMode(ir_pin, INPUT);

	DBG_PRINTLN("UserInputControl: Enabling IRin");
	remote.enableIRIn(); // Start the receiver's interrupt loop
	DBG_PRINTLN("UserInputControl: Enabled IRin");
}


void UserInputControl::poll() {
	// Check the buttons
	bool open = digitalRead(open_pin);
	bool close = digitalRead(close_pin);

	// Check the remote,
	// The first pressed button will be present in the results register.
	// We only resume listening if the output is retrieved.
	if(remote.decode(&remote_results)) {
		latest_signal = remote_results.value;
		new_signal_trigger = true;
		last_remote_signal_time = millis();
		remote.resume();
	}

	// Set internal state
	Button current_button;
	if (open && !close) {
		current_button = OPEN;
	} else if (!open && close) {
		current_button = CLOSE;
	} else if (open && close) {
		current_button = BOTH;
	} else {
		current_button = NONE;
	}

	// Detect rising/falling edges and store time-of-edge
	if (current_button != last_button) {
		last_edge_time = millis();

		// Save last button for future edge detection
		// (no need to do this if they are already the same)
		last_button = current_button;
	}


	// Save debounced input if it sticks (smoothing)
	// based on a timed threshold. (only if it makes sense)
	if (last_debounced_button != last_button && millis() - last_edge_time > DEBOUNCE_DELAY) {
		last_debounced_button = last_button;

		// Save the last button combo if it was an actual press
		if (last_button != NONE) {
			last_real_button = last_button;
			last_real_button_time = millis();
		}
	}
}

// Returns shortest time since last input event
unsigned long UserInputControl::time_since_input() {
	if (any_buttons_pressed()) { // This counts as current input
		return 0;
	} else {
		return millis() - max(last_remote_signal_time, last_real_button_time);
	}
}

/* BUTTONS */

Button UserInputControl::buttons_pressed() {
	poll();
	return last_debounced_button;
} 

Button UserInputControl::last_button_pressed() {
	poll();
	return last_real_button;
}

bool UserInputControl::open_pressed() {
	poll();
	return last_debounced_button == OPEN;
}

bool UserInputControl::close_pressed() {
	poll();
	return last_debounced_button == CLOSE;
}

bool UserInputControl::both_pressed() {
	poll();
	return last_debounced_button == BOTH;
}

bool UserInputControl::any_buttons_pressed() {
	poll();
	return last_debounced_button != NONE;
}

bool UserInputControl::home_pressed() {
	poll();
	return digitalRead(home_pin);
}

unsigned long UserInputControl::last_button_press_time() {
	return last_real_button_time;
}

unsigned long UserInputControl::time_to_last_press() {
	return millis() - last_real_button_time;
}


/* REMOTE */

// Returns true only if we have a new signal
bool UserInputControl::new_signal() {
	poll();
	return new_signal_trigger;
}

// Returns last signal we received
long UserInputControl::remote_signal() {
	poll();
	new_signal_trigger = false; // signal retrieved - no longer new.
	return latest_signal;
}

// Returns timestamp of last signal
unsigned long UserInputControl::get_last_signal_time() {
	return last_remote_signal_time;
}

unsigned long UserInputControl::time_to_last_signal() {
	return millis() - last_remote_signal_time;
}
