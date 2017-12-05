/*

Title: RGBDisplay Library
Author: Jason Cemra
Date: 02/12/2017

Description:

	This library asynchonously handles the state of an input system comprised
	of buttons and a remote.

*/

#ifndef RGBDISPLAY_H
#define RGBDISPLAY_H

#include "Arduino.h"
#include <IRremote.h>

// Allow the use of a debugging hook
#ifndef DEBUG
#define DEBUG true
#endif

#if DEBUG
#	define DBG_PRINT(...)    Serial.print(__VA_ARGS__)
#	define DBG_PRINTLN(...)  Serial.println(__VA_ARGS__)
#else
#	define DBG_PRINT(...)
#	define DBG_PRINTLN(...)
#endif

#define MIN_POLL_DELAY 5 // ms to wait before polling again.
#define DEBOUNCE_DELAY 30 // ms to wait before locking in a signal as true.


enum Button {
	OPEN,
	CLOSE,
	BOTH,
	NONE
};

class SensorInputControl {

};

class UserInputControl {
public:
	UserInputControl(short open_pin, short close_pin, short ir_pin);

	void poll(); // Updates internal state

	Button buttons_pressed();
	Button last_button_pressed();

	// Buttons
	bool open_pressed();
	bool close_pressed();
	bool both_pressed(); 
	bool any_buttons_pressed();

	unsigned long last_button_press_time();
	unsigned long time_to_last_press();

	// Remote
	bool is_receiving();
	long remote_signal();

private:
	// Pins:
	short open_pin;
	short close_pin;
	short ir_pin;

	// For debouncing:
	Button last_button = NONE; // Stores last button pressed (or none)
	Button last_real_button = NONE; // Stores last real button pressed (not none - for multi-click detection)
	Button last_debounced_button = NONE; // Stores last button pressed (or none) debounced

	unsigned long last_edge_time; // Stores the time of the last rising/falling edge.
	unsigned long last_poll_time; // Stores last time we polled buttons (don't want to overdo it)
	unsigned long last_real_button_time; // Stores last time we got a real button (i.e. not none)

	// For the remote:
	IRrecv remote;
	long last_signal;
	long latest_signal;
	decode_results remote_results;
};

#endif
