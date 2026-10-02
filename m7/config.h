// SPDX-License-Identifier: BSD-3-Clause
// source code based on the original dmc-lite sketch by Dyami Caliri
// at DZED Systems, Inc. (https://www.dragonframe.com)
// used with permission from the orginal author.
// Copyright 2023 by DZED Systems LLC
// Copyright 2026 by Limnmedia LLC

/**
 * \file config.h
 * \brief limnmoco configuration for the m7 core
 *
 * \note all signals are 3.3V TTL
 */

#ifdef ARDUINO_ARCH_MBED_GIGA

#define LOGIC_OUT_0 D40
#define LOGIC_OUT_1 D41

// set a pin for an e-stop switch. uses pull-up resistor, so switch needs to connect pin to ground.
#define KILL_SWITCH_PIN  D48
// if you want to use a 'normally closed' switch, uncomment the following line
//#define KILL_SWITCH_NORMALLY_CLOSED 1

// set a pin for a logic switch input. uses pull-up resistor, so switch needs to connect pin to ground.
//#define LOGIC_SWITCH_PIN  D49

#define PIN_DBG_0 D2
#define PIN_DBG_1 D3
#define PIN_DBG_2 D4
#define PIN_DBG_3 D5
#define PIN_DBG_4 D6
#define PIN_DBG_5 D7
#define PIN_DBG_6 D8
#define PIN_DBG_7 D9

#define PIN_7SEG_A  D46
#define PIN_7SEG_B  D38
#define PIN_7SEG_C  D43
#define PIN_7SEG_D  D47
#define PIN_7SEG_E  D49
#define PIN_7SEG_F  D44
#define PIN_7SEG_G  D41
#define PIN_7SEG_DP D45

#define PIN_7SEG_DIG1 D48
#define PIN_7SEG_DIG2 D42
#define PIN_7SEG_DIG3 D40
#define PIN_7SEG_DIG4 D39

#elif defined(ARDUINO_ARCH_MBED_PORTENTA)


#define LAST_ARDUINO_PIN_NUMBER LEDB + 1


#define LOGIC_OUT_0 LAST_ARDUINO_PIN_NUMBER + PD_4       // GPIO 2
#define LOGIC_OUT_1 LAST_ARDUINO_PIN_NUMBER + PD_5       // GPIO 3

// set a pin for an e-stop switch
//#define KILL_SWITCH_PIN  LAST_ARDUINO_PIN_NUMBER + PE_3  // GPIO 4

// set a pin for a logic switch input
//#define LOGIC_SWITCH_PIN  LAST_ARDUINO_PIN_NUMBER + PG_3  // GPIO 5

#else

#error "Board not supported for limnmoco sketch"

#endif
