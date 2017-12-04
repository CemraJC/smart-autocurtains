/*

Title:
Author: Jason Cemra
Date: 02/12/2017

Description: This library asynchonously handles the state of an RGB
LED system for the output of various signals.


*/

#include "RGBDisplay.h"

/*
Set up the display controller and set pin modes
*/
RGBDisplay::RGBDisplay(short red=5, short green=6, short blue=7) {
	pins[0] = red;
	pins[1] = green;
	pins[2] = blue;

	for (short i = 0; i < 3; ++i) {
		pinMode(pins[i], OUTPUT);
	}
}

void RGBDisplay::on(bool r, bool g, bool b) {
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


