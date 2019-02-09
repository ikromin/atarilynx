# Inputs Demo

This is a demo that shows how to read and buffer joystick inputs. Buffering is used so that inputs are not skipped between frames. Once an input is processed, its buffered state is cleared.

# Usage

The following macros provide all of the buffered joystick functionality. Use each one as if it was a boolean variable.

* BJOY_UP
* BJOY_DOWN
* BJOY_LEFT
* BJOY_RIGHT
* BJOY_A
* BJOY_B

Example:

```
if (BJOY_UP) {
	// up button was pressed
}
```

# Building

To build run ```make all```. This will produce a file ```inputs.o``` that can be run in an emulator.
