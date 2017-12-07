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
UserInputControl input(3, 4, 13, 12); // Open, Close, Home and IR pins
SensorInputControl sensors(A0, A1); // Light pin, Temp Pin
CurtainControl curtain(8,9,10,11);  // Stepper pins 1,2,3 and 4

unsigned int loop_pause = 1; // For controlling the speed of the loop

void setup() {

	if (DEBUGGING) { Serial.begin(9600); };

	DBG_PRINTLN("Starting initialization...");
	input.init();
	sensors.init();
	curtain.init();
	rgb_out.init();
	DBG_PRINTLN("Finished initialization.");

	// Manual blocking flash is OK
	rgb_out.solid(0, 1, 0);
	delay(200);
	rgb_out.off();

	DBG_PRINTLN("Homing Curtains.");
	home_curtains();
	DBG_PRINTLN("Homing Complete.");

	// if (curtain.settings.away == 0) { // Then it's the first run
	// 	rgb_out.solid(1, 0, 0);
	// 	record_remote();
	// 	record_away();
	// }

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

	// At least a 1 ms pause, for heat dissipation. 
	// Also for low power (sleep) mode.
	delay(loop_pause); 
}

void home_curtains() {
	// We expect that a home switch will eventually
	// be pressed! If it is not, then bad things may happen.
	while (!input.home_pressed()) {
		curtain.step(true);
	}
	curtain.set_home();
}