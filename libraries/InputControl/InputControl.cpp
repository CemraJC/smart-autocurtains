/*

Title: InputControl Library Definitions
Author: Jason Cemra
Date: 05/12/2017

*/

#include "InputControl.h"


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
	// Don't poll too often 
	if (millis() - last_poll_time < MIN_POLL_DELAY) {
		return;
	}

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

	// Keep track of the last time we polled
	last_poll_time = millis();
}

// Returns shortest time since last input event
unsigned long time_since_input() {
	return millis() - max(last_remote_signal_time, last_button_press_time);
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
unsigned long get_last_signal_time() {
	return last_remote_signal_time;
}