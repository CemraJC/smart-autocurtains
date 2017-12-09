/* Need header comment! */

#include <RGBDisplay.h>
#include <InputControl.h>
#include <CurtainControl.h>

// For Development
#define DEBUGGING true

#if DEBUGGING
#	define DBG_PRINT(...)    Serial.print(__VA_ARGS__)
#	define DBG_PRINTLN(...)  Serial.println(__VA_ARGS__)
#else
#	define DBG_PRINT(...)
#	define DBG_PRINTLN(...)
#endif

// Controls for sleep mode
#define ACTIVE_LOOP 0 // delay in ms between loops in an active state
#define SLEEP_LOOP 250 // delay in ms between loops in a sleep state
#define SLEEP_TIME 10000 // time in ms to wait after input before going into low power (sleep) mode

#define RECORD_LOOPS 3 // Number of times need to check a button before recording

#define SHORT_HOLD 2000 // ms to hold a button for to count as a "short hold"
#define LONG_HOLD 4000 // ms to hold a button for to count as a "long hold"

RGBDisplay rgb_out(5,6,7); // r, g, b pins
UserInputControl input(3, 4, 13, 11); // Open, Close, Home and IR pins
SensorInputControl sensors(A0, A1); // Light pin, Temp Pin
CurtainControl curtain(8,9,10,12);  // Stepper pins 1,2,3 and 4

unsigned int loop_pause = 1; // For controlling the speed of the loop

void setup() {

	if (DEBUGGING) { Serial.begin(9600); };

	DBG_PRINTLN("Starting initialization...");
	input.init();
	sensors.init();
	curtain.init(); // Settings will be read if they are present.
	rgb_out.init();
	DBG_PRINTLN("Finished initialization.");

	// Manual blocking flash is OK
	rgb_out.solid(0, 1, 0);
	delay(200);
	rgb_out.off();

	DBG_PRINTLN("Homing Curtains.");
	home_curtains();
	DBG_PRINTLN("Homing Complete.");

	if (curtain.settings.away == 0 && curtain.settings.remote_open == 0) { // Then it's the first run
		DBG_PRINTLN("First run. Learning signals and away position.");
		record_remote();
		record_away();
		curtain.trigger_write();
	} else if (curtain.settings.away == 0 && curtain.settings.remote_open != 0) { // Need to redo away position!!
		record_away();
		curtain.trigger_write();
	}

	serial_print_eeprom();
	DBG_PRINTLN();
	DBG_PRINTLN("Settings:");
	DBG_PRINTLN(curtain.settings.away);
	DBG_PRINTLN(curtain.settings.autodawn);
	DBG_PRINTLN(curtain.settings.autotemp);
	DBG_PRINTLN(curtain.settings.remote_open, HEX);
	DBG_PRINTLN(curtain.settings.remote_close, HEX);
	DBG_PRINTLN(curtain.settings.remote_cancel, HEX);
	DBG_PRINTLN(curtain.settings.remote_autodawn, HEX);
	DBG_PRINTLN(curtain.settings.remote_autotemp, HEX);

}

void loop() {

	// Control the low power (sleep) state of the system
	if (loop_pause != ACTIVE_LOOP && (input.time_since_input() < SLEEP_TIME || curtain.is_moving())) {
		loop_pause = ACTIVE_LOOP;
	} else if (loop_pause != SLEEP_LOOP && input.time_since_input() >= SLEEP_TIME && !curtain.is_moving()) {
		loop_pause = SLEEP_LOOP;
	}

	// Handle open/close/cancel requests
	if (input.open_pressed() || remote_open()) {
		curtain.open();
		rgb_out.pip(1, 1, 1, 1);
	} else if (input.close_pressed() || remote_close()) {
		curtain.close();
		rgb_out.pip(1, 1, 1, 1);
	} else if (input.both_pressed() || remote_cancel()) {
		curtain.cancel();
		rgb_out.pip(1, 1, 1, 1);
	}

	// Handle changes in settings
	if (remote_autodawn()) {
		curtain.settings.autodawn = !curtain.settings.autodawn;
		DBG_PRINTLN(String("Settings Autodawn to ") + String(curtain.settings.autodawn));
		curtain.trigger_write();
	} else if (remote_autotemp()) {
		curtain.settings.autotemp = !curtain.settings.autotemp;
		DBG_PRINTLN(String("Settings Autotemp to ") + String(curtain.settings.autotemp));
		curtain.trigger_write();
	}

	// Reset timer (reset entire system after long hold)
	if (input.both_pressed() && input.time_to_last_press() >= LONG_HOLD) {
		reset_system();
	}

	// Autodawn feature
	if (curtain.settings.autodawn) {

	}

	// Autotemp feature
	if (curtain.settings.autotemp) {

	}

	curtain.poll();
	rgb_out.update();

	// Variable delay for low power (sleep) mode.
	delay(loop_pause); 
}

// Records the remote buttons into the settings (and trigger a save)
void record_remote() {
	unsigned short seq_num = 1;
	long signal;
	long last_signal = 0;
	unsigned short signal_count = 0;

	rgb_out.solid(1, 0, 0);	

	while (seq_num <= 5) {
		if (input.new_signal()) {

			signal = input.remote_signal();

		    DBG_PRINT("Received: ");
		    DBG_PRINTLN(signal, HEX);

			// Make sure we haven't recorded the signal before:
			// (C++ is hard. This code is terrible, I know.)
			if (signal == curtain.settings.remote_open
			 || signal == curtain.settings.remote_close
			 || signal == curtain.settings.remote_cancel
			 || signal == curtain.settings.remote_autodawn
			 || signal == curtain.settings.remote_autotemp) {
				    DBG_PRINTLN("Tried to record an already recorded signal");
					signal_count = 0;

					// Yellow flash
			    	rgb_out.solid(1, 1, 0);
			    	delay(500);
			    	rgb_out.solid(1, 0, 0);
			    	continue;
			}

			if (signal == last_signal && signal_count < RECORD_LOOPS) {
			    DBG_PRINTLN("Success - inc. signal count.");
				++signal_count;

				// Single flash, ACK receive.
				rgb_out.solid(1, 1, 1);
		    	delay(25);
		    	rgb_out.solid(1, 0, 0);
			} else if (signal == last_signal && signal_count >= RECORD_LOOPS) {
				switch (seq_num) {
					case 1:
						curtain.settings.remote_open = signal;
						break;
					case 2:
						curtain.settings.remote_close = signal;
						break;
					case 3:
						curtain.settings.remote_cancel = signal;
						break;
					case 4:
						curtain.settings.remote_autodawn = signal;
						break;
					case 5:
						curtain.settings.remote_autotemp = signal;
						break;
				}

				signal_count = 0;
			    last_signal = 0;

				DBG_PRINT(String("Recorded signal for seq ") + String(seq_num) + String(" "));
				DBG_PRINTLN(signal, HEX);

				// Blocking flash (number of sequence)
				rgb_out.off();
				delay(250);
				rgb_out.pip(0, 0, 1, seq_num, 250);
				while (rgb_out.is_flashing()) {
					rgb_out.update();
				}
				rgb_out.solid(1, 0, 0);

				++seq_num;
				continue;
			} else if (signal != last_signal && signal_count > 0) { 
			    DBG_PRINTLN("Failure - reset signal count, start over.");
			    signal_count = 0;
			    last_signal = 0;

			    // Purple flash
				rgb_out.solid(1, 0, 1);
		    	delay(500);
				rgb_out.solid(1, 0, 0);
				continue;
			} else if (last_signal == 0) {
				++signal_count;
				last_signal = signal;

				// Single flash, ACK receive.
				rgb_out.solid(1, 1, 1);
		    	delay(25);
		    	rgb_out.solid(1, 0, 0);
			} else {

				// Single flash, ACK receive.
				rgb_out.solid(1, 1, 1);
		    	delay(25);
		    	rgb_out.solid(1, 0, 0);

				last_signal = signal;
			}
		}
	}

	rgb_out.off();

	DBG_PRINTLN("Finished recording remote. Signals:");
	DBG_PRINT("Open: ");
	DBG_PRINTLN(curtain.settings.remote_open, HEX);
	DBG_PRINT("Close: ");
	DBG_PRINTLN(curtain.settings.remote_close, HEX);
	DBG_PRINT("Cancel: ");
	DBG_PRINTLN(curtain.settings.remote_cancel, HEX);
	DBG_PRINT("Autodawn: ");
	DBG_PRINTLN(curtain.settings.remote_autodawn, HEX);
	DBG_PRINT("Autotemp: ");
	DBG_PRINTLN(curtain.settings.remote_autotemp, HEX);
	DBG_PRINTLN();
}

// Records the away position into the settings (and trigger a save)
void record_away() {
	rgb_out.solid(0, 0, 1);
	DBG_PRINTLN("Recording away position.");

	// Need to hold cancel on remote or buttons for SHORT_HOLD ms for this to exit
	while (true) {
		if (input.open_pressed() || remote_open()) {
			curtain.step(true);
		} else if (input.close_pressed() || remote_close()) {
			curtain.step(false);
		} else if (input.both_pressed() || remote_cancel()) {
			curtain.cancel();
		}

		if (input.both_pressed() && input.time_to_last_press() >= SHORT_HOLD) {
			break;
		} else if (remote_cancel() && input.time_to_last_signal() >= SHORT_HOLD) {
			break;
		}

		curtain.poll();
	}

	curtain.settings.away = curtain.get_location();
	DBG_PRINT("Away position set: ");
	DBG_PRINTLN(curtain.settings.away);

	rgb_out.off();
}

void home_curtains() {
	// We expect that a home switch will eventually
	// be pressed! If it is not, then bad things may happen.
	curtain.blind_rotate(true);
	rgb_out.pip(1, 0, 1);
	
	while (!input.home_pressed()) {
		curtain.poll();
		rgb_out.update();
	}

	rgb_out.off();
	curtain.cancel();
	curtain.set_home();
}

// Clears the EEPROM and restart the system
void reset_system() {
	rgb_out.solid(1, 1, 1);
	curtain.reset_settings();
	soft_reset();
}

void soft_reset() {
	asm volatile ("  jmp 0");
}


/*
Checking remote values
- Just some functions that return true if 
  the specified signal was received last and not acted on.
*/

bool remote_open() {
	return input.new_signal() && input.remote_signal() == curtain.settings.remote_open;
}

bool remote_close() {
	return input.new_signal() && input.remote_signal() == curtain.settings.remote_close;
}

bool remote_cancel() {
	return input.new_signal() && input.remote_signal() == curtain.settings.remote_cancel;
}

bool remote_autodawn() {
	return input.new_signal() && input.remote_signal() == curtain.settings.remote_autodawn;
}

bool remote_autotemp() {
	return input.new_signal() && input.remote_signal() == curtain.settings.remote_autotemp;
}


/*
DEBUGGING FEATURES
*/

void serial_print_eeprom() {
	for (int i = SETTINGS_ADDR; i < SETTINGS_END_ADDR; ++i) {
		Serial.print(String(i) + ": ");
		Serial.println(EEPROM[i]);
	}
}