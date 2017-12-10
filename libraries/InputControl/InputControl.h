/*

Title: RGBDisplay Library
Author: Jason Cemra
Date: 02/12/2017

Description:

	This library asynchonously handles the state of an input system comprised
	of buttons and a remote.

*/

#ifndef INPUTCONTROL_H
#define INPUTCONTROL_H

#include "Arduino.h"
#include <IRremote.h>

// Allow the use of a debugging hook
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

// #define MIN_POLL_DELAY 1 // ms to wait before polling again.
#define DEBOUNCE_DELAY 30 // ms to wait before locking in a signal as true.

// Sensor calibration constants for light detection
#define DARK_THRESHOLD 220
#define DUSK_GAP 40
#define LIGHT_THRESHOLD DARK_THRESHOLD + DUSK_GAP
#define LIGHT_DEBOUNCE_GAP 4 // Gap between light readings before we recalculate state
#define DAY_PHASE_DELAY /*30*60**/2*1000 // Time in ms between accepting a measured change in the current day phase


enum Button {
	OPEN,
	CLOSE,
	BOTH,
	NONE
};

// The day phases according to brightness
enum Brightness {
	DARK, // Low state, ready to go DUSK -> LIGHT
	LIGHT // High state, ready to go DUSK -> DARK
};

class SensorInputControl {
public:
	SensorInputControl(short light_pin, short temp_pin) : light_pin(light_pin), temp_pin(temp_pin) {};
	void init(); // IMPORTANT: Must call during setup to activate pin modes

	void poll();

	unsigned int get_light_reading();

	unsigned int get_temperature();

	unsigned long last_light_transition();

	bool is_dark();
	bool is_light();

private:
	// Pins
	short light_pin;
	short temp_pin;

	Brightness get_brightness();
	// To stop computation if difference isn't enough
	int last_light = 0;


	Brightness last_brightness = LIGHT; // Stores the last detected phase
	Brightness last_real_brightness = LIGHT; // Stores the last detected phase in DARK/LIGHT (i.e. not DUSK)
											 // This is for detecting edges only

	unsigned long last_poll_time; // Stops poll flooding
	unsigned long last_phase_edge_time; // Stores the time that we last accepted a DARK/LIGHT transition
	unsigned long phase_wait_trigger; // Stores the time that we last recorded a DARK/LIGHT transition (for phase delay debouncing)
};

class UserInputControl {
public:
	UserInputControl(short open_pin, short close_pin, short home_pin, short ir_pin) : open_pin(open_pin), close_pin(close_pin), home_pin(home_pin), ir_pin(ir_pin), remote(ir_pin) {};
	void init(); // IMPORTANT: Must call during setup to open the right interrupts.

	// Global 
	void poll(); // Updates internal state
	unsigned long time_since_input(); // Returns the time since the last input event

	// OnBoard Buttons
	Button buttons_pressed();
	Button last_button_pressed();

	bool open_pressed();
	bool close_pressed();
	bool both_pressed(); 
	bool any_buttons_pressed();

	// Homing switch (not debounced)
	bool home_pressed();

	unsigned long last_button_press_time();
	unsigned long time_to_last_press();

	// Remote
	bool new_signal();
	long remote_signal();
	unsigned long get_last_signal_time();
	unsigned long time_to_last_signal();

private:
	// Pins:
	short open_pin;
	short close_pin;
	short home_pin;
	short ir_pin;

	// For debouncing:
	Button last_button = NONE; // Stores last button pressed (or none)
	Button last_real_button = NONE; // Stores last real button pressed (not none - for multi-click detection)
	Button last_debounced_button = NONE; // Stores last button pressed (or none) debounced

	unsigned long last_edge_time; // Stores the time of the last rising/falling edge.
	unsigned long last_poll_time; // Stores last time we polled input (don't want to overdo it)
	unsigned long last_real_button_time; // Stores last time we got a real button (i.e. not none)

	// For the remote:
	IRrecv remote;
	bool new_signal_trigger; // Indicates if a new signal was received.
	long latest_signal; // Store the latest remote signal
	unsigned long last_remote_signal_time; // Store the last time a signal was received
	decode_results remote_results; // Class for storing the results of an IR input
};

#endif
