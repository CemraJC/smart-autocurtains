# InputControl

>Author: Jason Cemra

Handles checking for user input and providing hooks to respond to it.
Also provides a class to handle sensor input

## Usage

```cpp
// Set up the pins for user input devices.
UserInputControl input(open_button_pin, close_button_pin, homing_switch_pin, ir_remote_receiver_pin);

// Set up pins for sensor input devices
SensorInputControl sensors(light_sensor_pin, temperature_sensor_pin);
```

Note that every user input method is debounced. This means a rising/falling
edge must occur for at least 20ms before it is classed as an input stroke (for
buttons.)

## Methods for UserInputControl


### Global (defined on both)

```cpp
void poll();
```

Checks the current input state and updates the internal system. Very important
for debouncing and receiving remote input.

Each method that returns an input also fires a poll event (however, not more
often than the minimum poll waiting period)

```cpp
unsigned long time_since_input();
```

Returns the time (in ms) since the last input event


### Buttons

```cpp
bool open_pressed();
```

Returns true if the open button is pressed (but not both!)

```cpp
bool close_pressed();
```

Returns true if the close button is pressed (but not both!)

```cpp
bool both_pressed();
```

Returns true if both buttons are pressed.

```cpp
bool any_buttons_pressed();
```

Returns true if any button is pressed (on board buttons only).

```cpp
bool home_pressed();
```

Returns true if the homing switch is activated. This is not a debounced method.

The homing switch is a bit different from the other switches. It's not
technically a user input switch, and so it does not reset the last-input timer
and is not a debounced input method.

It still polls the system (since the opportunity exists) however it's state
doesn't come from the user input state machine and so a poll is unnecessary
(but still beneficial).

```cpp
Button buttons_pressed();
```

Returns "OPEN", "CLOSE", "BOTH" or "NONE" depending on the combination of buttons being pressed.

Warning: This method is debounced.

```cpp
Button last_button_pressed();
```

Returns the last real button we got (not NONE).

```cpp
unsigned long last_button_press_time();
```

Returns the ms timestamp of the last button press.

```cpp
unsigned long time_to_last_press();
```

Returns the number of ms to the last time a button press was detected.


### Remote Control

```cpp
bool is_receiving(); 
```

Returns true if we have cached a new signal.

```cpp
long remote_signal();
```

Returns the remote's signal that was received and resumes listening for new signal.

```cpp
unsigned long get_last_signal_time();
```

Returns the timestamp (in ms) of the last remote input event.


## Methods for SensorInputControl

```cpp
void poll();
```

Call this every loop to keep track of the readings and the debouncing asynchrounously.

```cpp
void init();
```

Call this in setup to activate the pin modes for temperature and light.

```cpp
Brightness get_brightness();
```

Returns DARK, DUSK or LIGHT. The light sensor is "debounced" (smoothed) by two
things: a large day-phase delay, and a mid-phase (DUSK) gap between LIGHT and
DARK to reduce false readings from overcast conditions.

```cpp
	unsigned int get_light_reading();
```

Returns the raw light reading from the photoresistor.

```cpp
	unsigned int get_temperature();
```

Returns the (calibrated) temperature reading in degrees celcius.

```cpp
	unsigned long last_light_transition();
```

Returns the last time we accepted a transition between LIGHT and DARK

```cpp
	bool is_light();
```

Returns if we think it's LIGHT outside.

```cpp
	bool is_dark();
```

Returns if we think it's DARK outside.