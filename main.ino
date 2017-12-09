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


	IRrecv irrecv(11);
	decode_results results;
	irrecv.enableIRIn(); // Start the receiver

	while (true) {
		if (irrecv.decode(&results)) {
			Serial.println(results.value, HEX);
			irrecv.resume(); // Receive the next value
		}
		delay(100);
	}

	DBG_PRINTLN("Homing Curtains.");
	home_curtains();
	DBG_PRINTLN("Homing Complete.");

	DBG_PRINTLN(curtain.settings.away);
	if (curtain.settings.away == 0) { // Then it's the first run
		DBG_PRINTLN("First run. Learning signals and away position.");
		record_remote();
		record_away();
	} 
}

void loop() {

	// Control the low power (sleep) state of the system
	if (loop_pause != ACTIVE_LOOP && (input.time_since_input() < SLEEP_TIME || curtain.is_moving())) {
		loop_pause = ACTIVE_LOOP;
	} else if (loop_pause != SLEEP_LOOP && input.time_since_input() >= SLEEP_TIME && !curtain.is_moving()) {
		loop_pause = SLEEP_LOOP;
	}


	if (input.open_pressed() /*|| input.remote_signal() == curtain.settings.remote_open*/) {
		curtain.open();
	} else if (input.close_pressed() /*|| input.remote_signal() == curtain.settings.remote_close*/) {
		curtain.close();
	} else if (input.both_pressed() /*|| input.remote_signal() == curtain.settings.remote_cancel*/) {
		curtain.cancel();
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

	rgb_out.solid(1, 0, 0);	

	while (false/*seq_num <= 5*/) {
		if (input.new_signal()) {

			signal = input.remote_signal();

			// Make sure we haven't recorded the signal before:
			// (C++ is hard. This code is terrible, I know.)
			if (signal == curtain.settings.remote_open
			 || signal == curtain.settings.remote_close
			 || signal == curtain.settings.remote_cancel
			 || signal == curtain.settings.remote_autodawn
			 || signal == curtain.settings.remote_autotemp) {
				    DBG_PRINTLN("Tried to record an already recorded signal");
			    	rgb_out.solid(1, 1, 0);
			    	continue;
			}

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

			DBG_PRINTLN(String("Recorded signal for seq ") + String(seq_num) + String(" ") + String(signal));

			// Blocking flash (number of sequence)
			rgb_out.pip(0, 0, 1, seq_num, 250);
			while (rgb_out.is_flashing()) {
				rgb_out.update();
			}
			rgb_out.solid(1, 0, 0);


			++seq_num;
		}
	}

	while (true) {
		if (input.new_signal()) {
			Serial.println(input.remote_signal(), HEX);
		}
		delay(50);
	}

	// curtain.trigger_write();
}

// Records the away position into the settings (and trigger a save)
void record_away() {

}

void home_curtains() {
	// We expect that a home switch will eventually
	// be pressed! If it is not, then bad things may happen.
	curtain.blind_rotate(true);
	while (!input.home_pressed()) {
		curtain.poll();
	}
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
	// volatile something?
}