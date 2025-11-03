#ifndef CONFIG_h
#define CONFIG_h

#include "release.h"

#define PARAM_IN_EEPROM 1
#define ENABLE_PROGMODE 1

#undef  DEBUG_KEYS
#undef  DEBUG_ADC

/* The user specific settings, like pin mappings or special configuration variables and sensitivities are stored in config.h.
   This file is meant for the << HALL-EFFECT SPACEMOUSE >>
   Please adjust your settings and save it as --> config.h <-- !
*/


/* Calibration Instructions
============================
Follow this file from top to bottom to calibrate your space mouse.
You can find some pictures for the calibration process here:
https://github.com/AndunHH/spacemouse/wiki/Ergonomouse-Build#calibration

Debugging Instructions
=========================
To activate one of the following debugging modes, you can either:
- Change STARTDEBUG here in the code and compile/download again
  OR
- Compile and upload the program. Go to the serial monitor and type the number and hit enter to select the debug mode.

Debug Modes:
------------
-1: Debugging off. Set to this once everything is working.
0:  Nothing...

1:  Report raw joystick values on 5V ref.    0-1023 raw ADC 10-bit values
10: Report raw joystick values on 2.56V ref. 0-1023 raw ADC 10-bit values
11: Calibrate / Zero the SpaceMouse and get a dead-zone suggestion (This is also done on every startup in the setup())

2:  Report centered joystick values. Values should be approximately -500 to +500, jitter around 0 at idle.
20: semi-automatic min-max calibration.

3:  Report centered joystick values. Filtered for deadzone. Approximately -350 to +350, locked to zero at idle, modified with a function.

4:  Report translation and rotation values. Approximately -350 to +350 depending on the parameter.
5:  Report debug 3 and 4 side by side for direct cause and effect reference.
6:  Report velocity and keys after possible kill-key feature
61: Report velocity and keys after kill-switch or ExclusiveMode
7:  Report the frequency of the loop() -> how often is the loop() called in one second?
8:  Report the bits and bytes send as button codes
9:  Report details about the encoder wheel, if ROTARY_AXIS > 0 or ROTARY_KEYS>0
*/

#define STARTDEBUG 0  // Can also be set over the serial interface, while the programm is running!

// Hardware uses HallEffect sensors instead of joystick sensors
#define HALLEFFECT

/* First Calibration: Hall effect sensors pin assignment, electrical check
=========================================================================== */
#define PINLIST \
  { A0,   A1,   A2,   A3,   A6,   A7,   A8,   A9 }
// HES0, HES1, HES2, HES3, HES6, HES7, HES8, HES9

#define INVERTLIST \
  {  0,    0,    0,    0,    0,    0,    0,    0 }
// HES0, HES1, HES2, HES3, HES6, HES7, HES8, HES9

/* Second calibration: Tune Deadzone
===================================== */
#define DEADZONE 5 // Recommended to have this as small as possible to allow full range of motion. // <<

/* Third calibration: Getting MIN and MAX values
================================================= */
//              {HES0, HES1, HES2, HES3, HES6, HES7, HES8, HES9}
#define MINVALS {-335, -323, -379, -305, -388, -305, -381, -422}
#define MAXVALS {118, 123, 144, 143, 113, 161, 103, 135}

/* Fourth calibration: Sensitivity
=================================== */
#define SENS_TX     0.55   // << консоль param::2
#define SENS_TY     0.62   // << param::3
#define SENS_PTZ    1.92   // sensitivity for positive translation z // <<
#define SENS_NTZ    1.30   // sensitivity for negative translation z // <<

#define GATE_NTZ    0     // gate for negative z (0 = off) // <<
#define GATE_RX     0     // Value under which rotX forced to zero // <<
#define GATE_RY     0     // Value under which rotY forced to zero // <<
#define GATE_RZ     0     // Value under which rotZ forced to zero // <<

#define SENS_RX     1.40  // << param::10
#define SENS_RY     1.36  // << param::11
#define SENS_RZ     0.81  // << param::12

/* Fifth calibration: Modifier Function
======================================== */
#define MODFUNC       1     // was 0 -> now “squared” curve // <<
#define MOD_A         1.15  // exponent "a"
#define MOD_B         1.25  // factor "b" // <<

/* Sixth Calibration: Direction
================================ */
// Switch between 0 or 1 as desired
#define INVX  0 // pan left/right                          // 3Dc: 0
#define INVY  1 // pan up/down                             // 3Dc: 1
#define INVZ  1 // zoom in/out                             // 3Dc: 1
#define INVRX 1 // Rotate around X axis (tilt front/back)  // 3Dc: 0 // <<
#define INVRY 0 // Rotate around Y axis (tilt left/right)  // 3Dc: 1 // <<
#define INVRZ 0 // Rotate around Z axis (twist left/right) // 3Dc: 1 // <<

#define SWITCHYZ 0
#define SWITCHXY 0

// ------------------------------------------------------------------------------------
// Hallsensor drift compensation
#define COMP_EN     1
#define COMP_NR     50
#define COMP_WAIT   200
#define COMP_MDIFF  4
#define COMP_CDIFF  50

/* Exclusive mode
================== */
#define EXCLUSIVE   0
#define EXCL_HYST   5

/* Prio-Z-Exclusive mode
========================= */
#define EXCL_PRIOZ 0

/* Key Support
=============== */
#define NUMKEYS 5//3 // 0

#define KEYLIST \
    {0, 1, 2, 14, 16} //{0, 1, 2}


/* Report KEYS over USB HID to the PC
 ------------------------------------- */
#define NUMHIDKEYS 5 //3 // 0

#define SM_MENU 0
#define SM_FIT 1
#define SM_T 2
#define SM_R 4
#define SM_F 5
#define SM_RCW 8
#define SM_1 12
#define SM_2 13
#define SM_3 14
#define SM_4 15
#define SM_ESC 22
#define SM_ALT 23
#define SM_SHFT 24
#define SM_CTRL 25
#define SM_ROT 26


// --- Combo timing (tunable) ---
#define FN_COMBO_WINDOW_MS   180   // окно ожидания второй кнопки (90..130 подбирай)
#define FN_STICKY_MS         140   // "липкость" Fn после отпускания (100..180)
#define KEY_PULSE_FRAMES       1   // сколько HID-репортов держать "нажатой" (2..4)
// --- Combo timing (tuneable) ---
#ifndef FN_COMBO_WINDOW_MS
#  define FN_COMBO_WINDOW_MS   110   // окно ожидания второй кнопки (80..130 под себя)
#endif
#ifndef FN_SOLO_DELAY_MS
#  define FN_SOLO_DELAY_MS      40   // небольшая задержка, чтобы Fn-соло не стреляла при попытке комбо
#endif


// Map first three pins to 1/2/3
// #define BUTTONLIST {SM_2, SM_1, SM_3} // << было {SM_T, SM_R, SM_F}
// Индексы модификаторов в массиве keyState[]
#define KEY_FN1_IDX 3
#define KEY_FN2_IDX 4

// --- Базовый слой (без Fn): 1,2,3 ---
#define BUTTONLIST      {SM_2,    SM_1,    SM_3,    SM_SHFT, SM_4}  // Fn1 solo=MENU, Fn2 solo=ESC

// --- Слой Fn1 (Fn1 + 1/2/3) ---
#define BUTTONLIST_FN1  {SM_FIT,  SM_MENU, SM_ROT,  SM_SHFT, SM_4}

// --- Слой Fn2 (Fn2 + 1/2/3) ---
#define BUTTONLIST_FN2  {SM_CTRL, SM_ESC, SM_ALT,   SM_SHFT, SM_4}


/* Kill-Key Feature
-------------------- */
#define NUMKILLKEYS 0
#define KILLROT 2
#define KILLTRANS 3

#if (NUMKILLKEYS > NUMKEYS)
#error "Number of Kill Keys can not be larger than total number of keys"
#endif
#if (NUMKILLKEYS > 0 && ((KILLROT > NUMKEYS) || (KILLTRANS > NUMKEYS)))
#error "Index of killkeys must be smaller than the total number of keys"
#endif

#define DEBOUNCE_KEYS_MS 200

/* Encoder Wheel
================ */
#define ENCODER_CLK 2
#define ENCODER_DT 3

#define ROTARY_AXIS 0
#define RAXIS_ECH 200
#define RAXIS_STR 200

#define ROTARY_KEYS 0
#define ROTARY_KEY_IDX_A 2
#define ROTARY_KEY_IDX_B 3
#define ROTARY_KEY_STRENGTH 19

/* LED support
=============== */
//#define LEDpin 5
// #define LEDinvert
//#define LEDRING 24
#define VelocityDeadzoneForLED 15
#define LEDclockOffset 0
#define LEDUPDATERATE_MS 150

/* Advanced debug output settings
================================= */
#define DEBUGDELAY 100
#define DEBUG_LINE_END "\r"
//define DEBUG_LINE_END "\r\n"

/* Advanced USB HID settings
============================= */
// #define ADV_HID_REL
// #define ADV_HID_JIGGLE

#endif // CONFIG_h
