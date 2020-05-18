// These two options are mutually exclusive, since they both use the same serial port!
#ifndef __CONFIG_H
#define __CONFIG_H

#define SERIAL_DEBUG 0
#define MIDI_HAIRLESS 1
#if (SERIAL_DEBUG && MIDI_HAIRLESS)
#error
#endif 

#define LED_TEST 0

#endif
