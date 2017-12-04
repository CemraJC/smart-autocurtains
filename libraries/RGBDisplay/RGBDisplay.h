/*

Title:
Author: Jason Cemra
Date: 02/12/2017

Description: This library asynchonously handles the state of an RGB
LED system for the output of various signals.


*/

#ifndef RGBDISPLAY_H
#define RGBDISPLAY_H

#include "Arduino.h"

// Holds all the state information for keeping
// track of (and controlling) a flash routine.
// Flash routine alternates an "on state" and all LEDs off 
// for user specified periods.
struct Flash {
	bool on_state[3] = {1,1,1}; // White by default
	unsigned int num_flashes = 1; // If zero, will just keep flashing
	unsigned long start_time = 0; // Automatically set when flash is started. 
								  // Used to calculate current position in the flash.
	unsigned int on_len = 1000;
	unsigned int off_len = 1000;
};

enum Output {
	FLASH,
	SOLID,
	OFF
};

class RGBDisplay {
public:
	RGBDisplay(short r_pin=5, short g_pin=6, short b_pin=7);

	void solid(bool r, bool g, bool b);
	void flash(bool r, bool g, bool b, unsigned int num_flashes = 0, unsigned int on_len = 1000, unsigned int off_len = 1000);
	void off();
	void update(); // Call every polling loop to update LED display based on current "action".

private:
	void on(bool r, bool g, bool b);
	short pins[3]; // Keeps track of pins
	bool state[3] = {0, 0, 0}; // Always reflects current LED state (all off by default)
	Output action = OFF; // Reflects current action being performed
};

#endif