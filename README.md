# Smart Auto-curtain System

A foundational system for controlling a stepper motor based on remote input,
ambient light readings and locally measured temperature.

Every curtain system is different, so some independent design is required.

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

