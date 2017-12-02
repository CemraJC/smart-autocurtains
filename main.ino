/* Need header comment! */

#include <CheapStepper.h>

CheapStepper stepper (8,9,10,11); 

#define BTN_OPEN 3
#define BTN_CLOSE 4


unsigned short num_pins = 2;
unsigned short pin_list[] = {BTN_OPEN, BTN_CLOSE};

void setup() {
	for (unsigned short i = 0; i < num_pins; ++i) {
		pinMode(pin_list[i], INPUT);
	}

	Serial.begin(9600);

	stepper.setRpm(10); 
}

void loop() {
	Serial.print(digitalRead(BTN_OPEN));
	Serial.println(digitalRead(BTN_CLOSE));

	stepper.moveDegrees (true, 360*5);

	delay(1000);
}