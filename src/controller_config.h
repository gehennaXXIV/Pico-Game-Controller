#ifndef CONTROLLER_CONFIG_H
#define CONTROLLER_CONFIG_H

// This include is REQUIRED for HID_KEY definitions
#include "class/hid/hid.h"

#define SW_GPIO_SIZE 11
#define LED_GPIO_SIZE 0
#define ENC_GPIO_SIZE 0
#define WS2812B_LED_SIZE 9
#define WS2812B_LED_ZONES 1
#define WS2812B_LEDS_PER_ZONE (WS2812B_LED_SIZE / WS2812B_LED_ZONES)

#ifdef PICO_GAME_CONTROLLER_C

const uint8_t SW_KEYCODE[] = {
    HID_KEY_1, HID_KEY_2, HID_KEY_3, HID_KEY_4, 
    HID_KEY_5, HID_KEY_6, HID_KEY_7, HID_KEY_8, 
    HID_KEY_9, HID_KEY_Q, HID_KEY_W
};

const uint8_t SW_GPIO[] = {
    15, 14, 12, 13, 11, 9, 10, 8, 7, 26, 6
};

const uint8_t LED_GPIO[] = { 27 };
const uint8_t WS2812B_GPIO = 28;

#endif

extern bool joy_mode_check;

#endif