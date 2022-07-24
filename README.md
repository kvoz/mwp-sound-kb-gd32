# mwp-sound-kb-gd32
Minimal working project for handling sound and keyboard threads (based on FSM) for GD32F103.

Project info:
* MCU: GD32F103RET6 (analog STM32F103RET6)
* GD firmware version: 2.2.2 (2021-12-16)

### Features

1. No dynamic allocations, all data full static.
1. Hardware abstracted, for poring to another MCU need to implement callback functions and register them.
1. Non blocking keyboard debounce control.
1. Sync between FSMs throught abstracted `queue` object.
1. Sound system plays predefined melody pattern. Simple expansion of known melodies.

([back to top](#top))


### Limitations

1. Maximal melody length is 256 notes.
1. Melodies supports only playing in 1st octave.
1. There is no support to change rhytm of melody or duration of exact note in runtime.
1. Rests between notes are in ms and must be calculated from selected rhytm.

([back to top](#top))

### Usage

1. In `max9768.c` setup `MAX9768_ADDR2` and `MAX9768_ADDR2` fields.
1. In `keyboard.c` setup number of columns and rows in matrix keyboard, and register symbols of keys. Also checkout `key_pressed_callback()` function.
1. In `melodies.h` register new melody IDs.
1. In `melodies.c` write any score you like by example.

([back to top](#top))