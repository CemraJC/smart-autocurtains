/* Need header comment! */

#include <RGBDisplay.h>
#include <InputControl.h>
#include <CurtainControl.h>


CheapStepper stepper (8,9,10,11); 

#define BTN_OPEN 3
#define BTN_CLOSE 4

// Controls for sleep mode
#define ACTIVE_LOOP 100 // delay in ms between loops in an active state
#define SLEEP_LOOP 250 // delay in ms between loops in a sleep state
#define SLEEP_TIME 10000 // time in ms to wait after input before going into low power (sleep) mode

RGBDisplay rgb_out(5,6,7); // r, g, b pins
UserInputControl input(3, 4, 13, 12); // Open, Close, Home and IR pins
SensorInputControl sensors(A0, A1); // Light pin, Temp Pin

unsigned short num_pins = 2;
unsigned short pin_list[] = {BTN_OPEN, BTN_CLOSE};

unsigned int loop_pause = 1; // For controlling the speed of the loop
bool last_btn_state = 0;
bool rotation_dir;

void setup() {
	for (unsigned short i = 0; i < num_pins; ++i) {
		pinMode(pin_list[i], INPUT);
	}

	Serial.begin(9600);

	input.init();
	sensors.init();

	// stepper.setRpm(10); 
	// rotation_dir = EEPROM[0]; // CW
	// rgb_out.flash(1, 0, 0, 10);
}

void loop() {

	// Control the low power (sleep) state of the system
	if (loop_pause != ACTIVE_LOOP && input.time_since_input() < SLEEP_TIME) {
		loop_pause = ACTIVE_LOOP;
	} else if (loop_pause != SLEEP_LOOP && input.time_since_input() >= SLEEP_TIME) {
		loop_pause = SLEEP_LOOP;
	}
	
	// if ((digitalRead(BTN_CLOSE) || digitalRead(BTN_OPEN)) && last_btn_state != 1) {
	// 	last_btn_state = 1;
	// 	rotation_dir = !rotation_dir;
	// 	EEPROM[0] = rotation_dir;
	// } else {
	// 	last_btn_state = 0;
	// }

	// stepper.moveDegrees(rotation_dir, 360);

	// rgb_out.update();

	Serial.print(sensors.is_dark());
	Serial.print(sensors.is_dusk());
	Serial.println(sensors.is_light());

	// At least a 1 ms pause, for heat dissipation. 
	// Also for low power (sleep) mode.
	delay(loop_pause); 
}