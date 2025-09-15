#ifndef CONTROLLER_CONFIG_H
#define CONTROLLER_CONFIG_H

#include <stdint.h>
#include <stdbool.h>   // For bool, true, false
#include "tusb.h"      // For HID_KEY_* constants

#define SW_GPIO_SIZE 8               // Number of switches
#define LED_GPIO_SIZE 10             // Number of switch LEDs
#define ENC_GPIO_SIZE 2              // Number of encoders
#define ENC_PPR 600                  // Encoder PPR
#define MOUSE_SENS 1                 // Mouse sensitivity multiplier
#define ENC_DEBOUNCE false           // Encoder Debouncing
#define SW_DEBOUNCE_TIME_US 8000     // Switch debounce delay in us
#define ENC_PULSE (ENC_PPR * 4)      // 4 pulses per PPR
#define REACTIVE_TIMEOUT_MAX 1000000 // HID to reactive timeout in us

#ifdef PICO_GAME_CONTROLLER_C

// Keybinds must match SW_GPIO_SIZE
const uint8_t SW_KEYCODE[SW_GPIO_SIZE] = {
    HID_KEY_D, HID_KEY_F, HID_KEY_J, HID_KEY_K,
    HID_KEY_C, HID_KEY_M, HID_KEY_1, HID_KEY_G
};

const uint8_t SW_GPIO[SW_GPIO_SIZE] = {
    13, 11, 9, 7, 12, 8, 0, 10
};

// Encoder pins
const uint8_t ENC_GPIO[ENC_GPIO_SIZE] = {15, 5};      
const bool ENC_REV[ENC_GPIO_SIZE] = {false, false};  // Reverse encoders

#endif // PICO_GAME_CONTROLLER_C

// Global mode flag
extern bool joy_mode_check;

// Function declarations
void joy_mode(void);
void key_mode(void);
void debounce_mode(void);
void update_inputs(void);
void update_lights(void);

#endif // CONTROLLER_CONFIG_H
