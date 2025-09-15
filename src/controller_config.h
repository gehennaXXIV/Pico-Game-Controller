#ifndef CONTROLLER_CONFIG_H
#define CONTROLLER_CONFIG_H

#include <stdint.h>
#include <stdbool.h>

#define SW_GPIO_SIZE 8               // Number of switches
#define LED_GPIO_SIZE 10             // Number of switch LEDs
#define ENC_GPIO_SIZE 2              // Number of encoders
#define ENC_PPR 600                  // Encoder PPR
#define MOUSE_SENS 1                 // Mouse sensitivity multiplier
#define ENC_DEBOUNCE false           // Encoder Debouncing
#define SW_DEBOUNCE_TIME_US 8000     // Switch debounce delay in us
#define ENC_PULSE (ENC_PPR * 4)      // 4 pulses per PPR
#define REACTIVE_TIMEOUT_MAX 1000000 // HID to reactive timeout in us
// #define WS2812B_LED_SIZE 10
// #define WS2812B_LED_ZONES 2
// #define WS2812B_LEDS_PER_ZONE WS2812B_LED_SIZE / WS2812B_LED_ZONES

#ifdef PICO_GAME_CONTROLLER_C

// Switch GPIO pins
const uint8_t SW_GPIO[SW_GPIO_SIZE] = {13, 11, 9, 7, 12, 8, 0, 10};

// Encoder pins
const uint8_t ENC_GPIO[ENC_GPIO_SIZE] = {0, 2};
const bool ENC_REV[ENC_GPIO_SIZE] = {false, false};

// Uncomment these if you want LEDs later
// const uint8_t LED_GPIO[LED_GPIO_SIZE] = {5,7,9,11,13,15,17,19,21,26};
// const uint8_t WS2812B_GPIO = 28;

#endif // PICO_GAME_CONTROLLER_C

extern bool joy_mode_check;

#endif // CONTROLLER_CONFIG_H
