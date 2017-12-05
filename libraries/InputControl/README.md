# InputControl

Handles checking for user input and providing hooks to respond to it.
Also provides a class to handle sensor input

## Usage

```cpp
// Set up the pins for user input devices.
UserInputControl input(open_button_pin, close_button_pin, ir_remote_receiver_pin);

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

Returns true if any button is pressed.


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

## Methods for SensorInputControl