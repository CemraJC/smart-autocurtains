/*

Title: RGBDisplay Library Definitions
Author: Jason Cemra
Date: 04/12/2017

*/

#include "RGBDisplay.h"

/*
Set up the display controller and set pin modes
*/
void RGBDisplay::init() {
	for (short i = 0; i < 3; ++i) {
		pinMode(pins[i], OUTPUT);
	}

	if (DEBUGGING) {Serial.println("RGBDisplay set up complete.");}
}

void RGBDisplay::update() {
	// The only action that needs asynchronous updates
	if (action == FLASH) {
		// we know current state

		// calculate what it should be at this time
		unsigned long elapsed_time = millis() - flash_spec.start_time;

		if (elapsed_time < 0) { // Then the millis has rolled back to 0.
			// Discard reading and restart the flash now.
			// May experience minor jumpiness for a single cycle.
			if (DEBUGGING) {Serial.println("Millis has rolled to 0 - resetting flash start.");}
			flash_spec.start_time = millis();
			return;
		}

		unsigned int flash_len = flash_spec.on_len + flash_spec.off_len;

		// Find out how many flashes we've done and time since last one
		unsigned long total_flashes = elapsed_time / flash_len; // an integer!

		// Stop flashing if we're at / over(how?) the limit
		// If it's 0, there is no limit.
		if (flash_spec.num_flashes != 0 && total_flashes >= flash_spec.num_flashes) {
			off(); // Cancels flash mode
			return;
		}

		unsigned int time_since_last = elapsed_time % flash_len;

		// Find out the phase of the flash we're in and update state if necessary
		if (time_since_last <= flash_spec.on_len && !equal(state, flash_spec.on_state))  // if we should be in on phase and we're not...
		{ 
			if (DEBUGGING) {Serial.println("RGB LED Flash On");}
			set_state(flash_spec.on_state[0], flash_spec.on_state[1], flash_spec.on_state[2]);
		} 
		else if (time_since_last > flash_spec.on_len && !equal(state, flash_spec.off_state)) // we should be in off phase and we're not...
		{ 
			if (DEBUGGING) {Serial.println("RGB LED Flash Off");}
			set_state(flash_spec.off_state[0], flash_spec.off_state[1], flash_spec.off_state[2]);
		}
	}
}

void RGBDisplay::solid(bool r, bool g, bool b) {
	if (r || g || b) { // Need at least one to be on before solid
		if (DEBUGGING) {Serial.println("RGB LED solid initialized.");}
		action = SOLID; // Set current action being performed
		set_state(r, g, b);
	} else {
		off();
	}
}

void RGBDisplay::flash(bool r_on, bool g_on, bool b_on, unsigned int num_flashes=0, unsigned int on_len=1000, unsigned int off_len=1000, bool r_off=0, bool g_off=0, bool b_off=0) {
	if (DEBUGGING) {Serial.println("RGB LED flash initialized.");}
	action = FLASH;
	flash_spec.on_state[0] = r_on;
	flash_spec.on_state[1] = g_on;
	flash_spec.on_state[2] = b_on;
	flash_spec.off_state[0] = r_off;
	flash_spec.off_state[1] = g_off;
	flash_spec.off_state[2] = b_off;
	flash_spec.num_flashes = num_flashes;
	flash_spec.start_time = millis();
	flash_spec.on_len = on_len;
	flash_spec.off_len = off_len;
}


void RGBDisplay::off() {
	if (DEBUGGING) {Serial.println("RGB LED turned off.");}
	action = OFF; // Set current action
	set_state(0, 0, 0); // Turn all LEDs to state 0
}

// Returns true or false based on whether or not the LED is on/doing something
bool RGBDisplay::active() {
	return action != OFF;
}

// Only returns true if a flashing cyclbe is running
bool RGBDisplay::is_flashing() {
	return action == FLASH;
}

void RGBDisplay::set_state(bool r, bool g, bool b) {
	// Set the current state
	state[0] = r;
	state[1] = g;
	state[2] = b;

	// Make LED reflect current state
	for (short i = 0; i < 3; ++i) {
		if (state[i]) {
			digitalWrite(pins[i], HIGH);
		} else {
			digitalWrite(pins[i], LOW);
		}
	}
}

// Check if two state arrays are equal
bool RGBDisplay::equal(bool array1[], bool array2[]) {
    for(int i = 0; i < 3; ++i) {
        if(array1[i] != array2[i]) {
            return false;
        }
    }
    return true;
}

