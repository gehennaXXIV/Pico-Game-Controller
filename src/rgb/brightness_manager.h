#ifndef BRIGHTNESS_MANAGER_H
#define BRIGHTNESS_MANAGER_H

#include "pico/stdlib.h"
#include "controller_config.h"

static uint8_t global_brightness = 30;
static uint8_t light_mode = 0; // 0=Ping-Pong, 1=Breathing

static inline void handle_brightness_shortcuts() {
    static uint32_t last_tick = 0;
    uint32_t now = to_ms_since_boot(get_absolute_time());

    if (now - last_tick > 50) {
        
        // --- HOLD BUTTON 11 (Index 10): BRIGHTNESS ---
        if (!gpio_get(SW_GPIO[10])) {
            if (!gpio_get(SW_GPIO[3])) { // Button 4 (Index 3) Decrease
                if (global_brightness >= 5) global_brightness -= 5;
                else global_brightness = 0;
            }
            if (!gpio_get(SW_GPIO[5])) { // Button 6 (Index 5) Increase
                if (global_brightness <= 120) global_brightness += 5;
                else global_brightness = 125;
            }
        }

        // --- HOLD BUTTON 10 (Index 9): MODE SWITCH ---
        if (!gpio_get(SW_GPIO[9])) {
            if (!gpio_get(SW_GPIO[1])) light_mode = 0; // Button 2 (Index 1) -> Ping-Pong
            if (!gpio_get(SW_GPIO[8])) light_mode = 1; // Button 9 (Index 8) -> Breathing
        }

        last_tick = now;
    }
}

static inline uint8_t get_global_brightness() { return global_brightness; }
static inline uint8_t get_light_mode() { return light_mode; }

#endif