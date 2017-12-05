# RGBDisplay

>Author: Jason Cemra

Controls an RGB LED or a combination of red, green and blue LEDs.

## Usage

Initialization:

```cpp
// Initialize the pins
RGBDisplay rgb(red_led_pin, green_led_pin, blue_led_pin);
```

Methods:

```cpp
	void flash(bool r_on, bool g_on, bool b_on, unsigned int num_flashes=0, 
			   unsigned int on_len=1000, unsigned int off_len=1000, 
			   bool r_off=0, bool g_off=0, bool b_off=0)
```

This is the longest method for initializing a two-phase flashing sequence. You
can define an "on_state" and an "off_state" (which is literally off by
default) and you can control the length of time (in ms) in which the states
remain on. You can also specify a number of flashes before the system
automatically turns off.

*Tip:* You can use flash to turn the LED on (solid) for a period of time by
setting the on_len to the desired time and num_flashes to 1.

**Note:** You must call the `update()` method in a polling loop to ensure the
flashes actually update.

```cpp
	void pip(bool r, bool g, bool b, unsigned int num_flashes=0, int between=1200)
	void blink(bool r, bool g, bool b, unsigned int num_flashes=0, int between=2000)
```

The `pip()` and `blink()` methods are short aliases for `flash()` that define
the off_state to be literally off and pre-defined on_state lengths of 30ms and
230ms for a "pip" and a "blink".

Between controls how long between pips or blinks if multiple are active.

```cpp
	void solid(bool r, bool g, bool b)
```

Turns the LED on with the given state - the values of `r`, `g` and `b` (either
1 or 0) choose whether or not the red, green or blue LED is lit.

These values are digital (no colour mixing)

```cpp
bool active()
```

Returns true if the LED is doing something, and false otherwise.

```cpp
bool is_flashing()
```

Returns true only if the LED is flashing.

```cpp
void off()
```

Turns off the LED completely and cancels any current action.

```cpp
void update()
```

Must be called in a polling loop. If issues with timing arise, it is possible
that the polling loop takes too long to complete. As long as the poll cycle is
faster than the individual flash phases, you should not experience dramatic
desyncing or skipping.
