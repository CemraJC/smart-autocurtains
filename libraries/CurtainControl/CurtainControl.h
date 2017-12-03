/*

Title:
Author: Jason Cemra
Date: 02/12/2017

Description: This library handles the controlling of a stepper motor on a
pulley system with a homing limit switch. It manages the settings for the
program and is also responsible for providing output to the user in the
form of variously colored LED flashes.


*/

#ifndef CURTAINCONTROL_H
#define CURTAINCONTROL_H

#include "Arduino.h"

// Constants

#define SETTINGS_ADDR 230 // The starting address to write EEPROM settings to
#define MAX_SETTINGS 255 - SETTINGS_ADDR // Total limit of setting bits we can have

const struct s {

};



#define OPEN_DIRECTION true // Direction to drive the stepper to open the curtains (true for clockwise, false for CCW)
							// This is also the direction that the stepper will run to trip the homing switch



#endif