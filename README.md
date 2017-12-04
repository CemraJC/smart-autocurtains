# Smart Auto-curtain System

A foundational system for controlling a stepper motor based on remote input,
ambient light readings and locally measured temperature.

Every curtain system is different, so some independent design is required.

**Please read this whole document before building! Seriously!** (I mean, c'mon,
it's not that long - it'll save you a bunch of time in the setup)

## What it does

The basic system will open or close curtains based on remote control input. In
the event of remote control failure, the system has two integrated buttons to
operate the curtains.

**However,** the system can do much more than simple remote operation. If
"Auto-brightness control" is enabled, the system will close the blinds if
outside light levels exceed a pre-determined threshold. This is to assist with
sleeping in bright-early environments.

Also, depending on the local temperature (and your currently active settings),
the system may open the blinds to allow sunlight or air circulation.

Note that the system does not use the local time to operate, because the
hardware required adds unnecessary complexity to the setup and user operation of
the system.


## System components:

1. Arduino Nano
    * Could be substituted for a different type of Arduino
    * Minimum specs:
    * Must have a regulated power supply
2. Stepper Motor and controller (more info - todo)
3. 5V power supply and cable (more info - check max rating for stepper)
4. 2 momentary push buttons
5. 1 photoresistor
6. 1 temperature sensor
7. 1 IR LED (for receiving remote signals)
8. 1 RGB LED (or 3 separate red, green, blue LEDs - diffused!)
9. Arbitrary IR remote (buttons will be programmed in)
10. Perforated circuit board (minimum 300x250)
11. Solid-core wire (cleaner than stranded)
12. Resistors (220, 1k, 10k)
13. Cardboard / MDF (more info when final casing completed)
14. Bump-press limit switch (for homing the system)

## Tools required:

1. Soldering Iron and Small Fan (to evacuate soldering smoke)
2. Wire strippers (optional, but highly recommended)
3. Screwdriver

## Building (TODO)


# Set-up procedure

The Smart Autocurtains require the following setup:

* Set the remote buttons
* Set the "away" position

These settings are saved in the Arduino's non-volatile memory, so the settings
are not wiped in a loss-of-power event. In other words, you should only have
to set up the system once.

Before attempting setup, make sure that the power is on. The system should
flash a green LED once when the power is turned on. If it is the first run, a
red LED should turn on next.

## Setting remote buttons

When you first run the system (or run it after a user-reset), you will need to
record the buttons on your remote. Every remote is different, however this
should still be a simple procedure. If you have difficulties, try changing the
remote you are using and making sure the remote is functional.

To check that your remote has battery, point a smartphone camera at the IR LED
on the front. Then, press a button. If the camera shows a white flash, then
the remote is functional.

To record which buttons do what, follow these steps:

1. Observe that a red LED is lit up on the device (first run only)
1. Press the button on the remote that you want to correspond to "open"
1. Observe that the system flashes a blue LED exactly once
1. Choose and press the "close" button on the remote
1. Observe that the system flashes a blue LED exactly twice
1. Choose and press the "cancel" button
1. Observe that the system flashes a blue LED exactly three times
1. Choose and press the "auto-dawn" button (to enable/disable closing the curtains when it gets bright in the morning)
1. Observe that the system flashes a blue LED exactly four times
1. Choose and press the "auto-temp" button (to enable/disable force-opening the curtains when the temperature exceeds 28 degrees C)
1. Observe that the system flashes a blue LED exactly five times and the curtains proceed to the home position

**Important:** on first run, be prepared to quickly power down the system in
the event that the homing switch does not function correctly. If the stepper
is allowed to run past home, it may stall and/or cause damage to the device or
your curtains.

Now is a good time to mention that we take no responsibility for any damages
caused by the system (although, to be fair, it's pretty unlikely that anything
seriously bad could happen).


## Setting home and away positions

#### What are home and away positions?

The curtains have two key positions, "home" and "away". The home position is
set by where the homing switch is placed. Depending on how the system is set
up, home and away could be in very different places. Regardless, home is the
position the curtains should be in when _open_. Home is set automatically by
the system, assuming the limit switch is properly in place.

Once home is found (the system re-homes every time on startup), the "away"
position must be set by the user (if it has not been already). 

#### How to set the away position

To set the away position, wait for the "red blue" LED sequence (purple on an
RGB system) and then proceed to press and hold the open/close buttons (on the
remote, or the onboard buttons) to set the away position. If the directions
are wrong (i.e the open button closes the curtains) then you will need to
switch the rails to which the curtains are clipped.

_Note:_ Holding down a button may not work on some remotes. In this case, you
will need to repeatedly press the open/close remote buttons to step the
curtains into the desired position. Alternatively, press and hold the onboard
system buttons (this should always work).

Once you are happy with the curtain's position as the "away" position, press
the "cancel" button you set up earlier. The away position is now set, it
should not need to be set again (unless you reset the system). 

## All done!

When the away position is set successfully, the device should flash a green
LED. Your Smart Autocurtains are now ready for use!


# How to use the Smart Autocurtains

Once the curtains are set up according to the above procedure, you can operate
the curtains in the following ways:

## Using the remote

With a single press of the open or close buttons, the curtains will travel the
full distance to the home/away position respectively. If you only want the
curtains, say half-open, then press the cancel button halfway through the
motion.

Once a motion is canceled, a subsequent motion will pick back up from where
the curtains currently are.

## Using the on-board buttons

There are two buttons, one to open, and one to close. If both buttons are
pressed at once, the curtains will cancel their current motion (assuming there
is one being made).

If both buttons are pressed and held for 4 seconds, the entire system should
reset - including clearing all settings, remote buttons and positions. The
system will need to be setup from scratch if reset in this way.

## With the "smart" features

There are two smart features, "autodawn control" and "autotemp control". These
smart features operate the curtains without user input in response to readings
from light and temperature sensors.

To **enable/disable** these features individually, press the corresponding buttons
on the remote that were recorded during setup and observe that the system
should flash a red or green LED for "off" or "on" respectively.

## Autodawn

This feature is the reason this system exists. Where I (the developer) live,
it gets fairly bright at around 4 or 5am. I prefer to wake up at 7am, but the sun
won't let me. You get the idea.

The system uses a light sensor to determine if it is currently night. It does
this by waiting for the light levels to drop below a "night-time" threshold
for a minimum of 30 minutes. If it is night (or the system _thinks_ it is),
then the curtains will open, to allow air circulation.

Then, when light levels exceed the "night-time" threshold level for at least
30 minutes, the system will close the blinds to prevent early-brightness from
waking you up.

## Autotemp

Having the curtains closed for brightness is only good as long as the
temperature is bearable. Based on external research, 28 degrees C is a general
threshold at which humans find it difficult to sleep. For this reason, if
autotemp is enabled, the curtains will be forced to open to allow air
circulation - regardless of autodawn. Of course, user input will override the
auto-temp setting (for up to 1 hour) unless autotemp is disabled, and then re-
enabled.

Whether or not this feature makes sense for you depends on your unique
circumstance. It also assumes that your windows are actually open - otherwise,
all it does is let in sunlight and make things worse!


# Troubleshooting

## Deep cycle the power

If the system is malfunctioning, follow these steps to try and resolve the issue:

1. Turn the system off with the power switch, then turn off the switch at the wall.
2. Wait for 5 seconds to ensure all power has drained from the system
3. Turn the wall switch back on (nothing should be happening yet)
4. Turn the device's power switch back on

Now, you should see the system flash a green LED twice and the system should
go to home.

## Well, that didn't fix it.

Press and hold the onboard buttons for 4 seconds. The system should flash a
red LED twice, then reset itself - clearing all settings. Then, go through the
setup procedure again and see if the system is now functioning correctly.

You should not have to do this often/at all. If you find yourself regularly
doing this, then check that the stepper is rotating _continuously_ (i.e. no
jumpiness or skips) when opening/closing the curtains. If it does not rotate
continuously, then you will need to reduce the torque on the stepper by either
adjusting the curtains, so they move with less friction, or by upgrading the
stepper motor to one with higher torque. A stepper upgrade is difficult work,
and is left as an exercise only to advanced users. 

## Something is probably broken.

If there are no green flashes on startup, confirm that the LEDs are functional.
Still no flashes, then either there is something wrong with the code, or the
Arduino is malfunctioning - possibly due to loose connections, or a short
circuit. If you have made modifications to the code, you will need to debug it
yourself.

If you do see two green flashes, the system should move the curtains
to the home position. If the curtains do not move, and are not already at the
home position, check the wiring to the stepper motor and confirm that power is
being correctly applied.

If the wiring is correct, and still no motion, use your fingers to test the
temperature of the motor and all the IC's (integrated circuit chips). They
should all be at room temperature, or only slightly warm. If any of the chips
are too hot to touch, immediately power down the system and triple check the
wiring, paying careful attention to potential short circuits due to faulty
resistors/capacitors.

If the curtains move, but do not stop when they reach home (or they home too
early/at random positions) power down the system and confirm that the limit
switch is functioning correctly and is properly hooked up to the Arduino.

If the curtains do not reliably return to the home and away positions, go to
the previous section to check that the stepper is functioning correctly.

