/* Need header comment! */

#include <CheapStepper.h>
#include <EEPROM.h>

CheapStepper stepper (8,9,10,11); 

#define BTN_OPEN 3
#define BTN_CLOSE 4


unsigned short num_pins = 2;
unsigned short pin_list[] = {BTN_OPEN, BTN_CLOSE};

bool last_btn_state = 0;
bool rotation_dir;

void setup() {
	for (unsigned short i = 0; i < num_pins; ++i) {
		pinMode(pin_list[i], INPUT);
	}

	Serial.begin(9600);

	stepper.setRpm(10); 
	rotation_dir = EEPROM[0]; // CW
}

void loop() {
	
	if ((digitalRead(BTN_CLOSE) || digitalRead(BTN_OPEN)) && last_btn_state != 1) {
		last_btn_state = 1;
		rotation_dir = !rotation_dir;
		EEPROM[0] = rotation_dir;
	} else {
		last_btn_state = 0;
	}

	stepper.moveDegrees(rotation_dir, 360);

	delay(1000);
}