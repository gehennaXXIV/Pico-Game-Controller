#ifndef CONTROLLER_CONFIG_H
#define CONTROLLER_CONFIG_H

#define SW_GPIO_SIZE 9                // Number of switches
#define LED_GPIO_SIZE 1               // Number of switch LEDs
#define ENC_GPIO_SIZE 2               // Number of encoders
#define ENC_PPR 24                    // Encoder PPR
#define MOUSE_SENS 5                  // Mouse sensitivity multiplier
#define ENC_DEBOUNCE true             // Encoder Debouncing
#define SW_DEBOUNCE_TIME_US 4000      // Switch debounce delay in us
#define ENC_PULSE (ENC_PPR * 4)       // 4 pulses per PPR
#define REACTIVE_TIMEOUT_MAX 1000000  // HID to reactive timeout in us
#define WS2812B_LED_SIZE 10           // Number of WS2812B LEDs
#define WS2812B_LED_ZONES 2           // Number of WS2812B LED Zones
#define WS2812B_LEDS_PER_ZONE \
  WS2812B_LED_SIZE / WS2812B_LED_ZONES  // Number of LEDs per zone

#ifdef PICO_GAME_CONTROLLER_C

// MODIFY KEYBINDS HERE, MAKE SURE LENGTHS MATCH SW_GPIO_SIZE
const uint8_t SW_KEYCODE[] = {HID_KEY_D, HID_KEY_F, HID_KEY_J, HID_KEY_K,
                              HID_KEY_C, HID_KEY_M, HID_KEY_A, HID_KEY_B};
const uint8_t SW_GPIO[] = {
    13, 11, 9, 7, 12, 8, 0, 10,
};
const uint8_t LED_GPIO[] = {
    2,
};
const uint8_t ENC_GPIO[] = {14, 5};      // L_ENC(0, 1); R_ENC(2, 3)
const bool ENC_REV[] = {false, false};  // Reverse Encoders
const uint8_t WS2812B_GPIO = 28;

#endif

extern bool joy_mode_check;

#endif
