/*

Title: RGBDisplay Library
Author: Jason Cemra
Date: 02/12/2017

Description:

	This library asynchonously handles the state of an RGB LED system for the
	output of various signals. The main function is to illuminate a single RGB
	LED with a highly customizeable two-phase flashing pattern. A number of preset patt

	The LED can also simply just be turned on.  
	Every action can be performed in a variety of colors, but is restricted to
	digital values (i.e. no PWM color mixing).

*/

#ifndef RGBDISPLAY_H
#define RGBDISPLAY_H

#include "Arduino.h"

// Allow the use of a debugging hook
#ifndef DEBUGGING
#define DEBUGGING true
#endif

// Holds all the state information for keeping
// track of (and controlling) a flash routine.
// Flash routine alternates an "on state" and all LEDs off 
// for user specified periods.
struct Flash {
	bool on_state[3] = {1,1,1}; // White by default
	bool off_state[3] = {0,0,0}; // All off by default
	unsigned int num_flashes = 1; // If zero, will just keep flashing forever until cancelled.
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
	RGBDisplay(short r_pin=5, short g_pin=6, short b_pin=7) : pins({r_pin, g_pin, b_pin}) {};

	void init();

	// Toggle between two states asynchronously with parameters
	void flash(bool r_on, bool g_on, bool b_on, unsigned int num_flashes=0, unsigned int on_len=1000, unsigned int off_len=1000, bool r_off=0, bool g_off=0, bool b_off=0);

	// Shorter flash aliases
	void pip(bool r, bool g, bool b, unsigned int num_flashes=0, int between=1200) { // 30ms on-time
		flash(r, g, b, num_flashes, 30, between);
	}; 
	void blink(bool r, bool g, bool b, unsigned int num_flashes=0, int between=2000) { // 230ms on-time
		flash(r, g, b, num_flashes, 230, between);
	};

	// Simply turn on the LED with given color combo
	void solid(bool r, bool g, bool b);
	
	// Returns true or false based on whether or not the LED is on/doing something
	bool active();
	bool is_flashing(); // Only returns true if a flashing cycle is running

	void off();
	void update(); // Call every polling loop to update LED display based on current "action".

private:
	void set_state(bool r, bool g, bool b);
	bool equal(bool array1[], bool array2[]); // Check if the contents of two boolean arrays are equal
	short pins[3]; // Keeps track of pins
	bool state[3] = {0, 0, 0}; // Always reflects current LED state (all off by default)
	Output action = OFF; // Reflects current action being performed
	Flash flash_spec; // If a flashing pattern is active, this specifies everything.
};

#endif