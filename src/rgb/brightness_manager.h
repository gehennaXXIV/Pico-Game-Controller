#ifndef BRIGHTNESS_MANAGER_H
#define BRIGHTNESS_MANAGER_H

#include "pico/stdlib.h"
#include "controller_config.h"

static uint8_t global_brightness = 30;

static inline void handle_brightness_shortcuts() {
    // 1. Check if Button 11 (Index 10) is held
    if (!gpio_get(SW_GPIO[10])) {
        static uint32_t last_tick = 0;
        uint32_t now = to_ms_since_boot(get_absolute_time());

        // 2. Only run every 50ms so the change isn't instant
        if (now - last_tick > 50) {
            
            // DECREASE: Button 4 (Index 3)
            if (!gpio_get(SW_GPIO[3])) {
                if (global_brightness >= 5) {
                    global_brightness -= 5;
                } else {
                    global_brightness = 0;
                }
            }
            
            // INCREASE: Button 6 (Index 5)
            if (!gpio_get(SW_GPIO[5])) {
                if (global_brightness <= 95) {
                    global_brightness += 5;
                } else {
                    global_brightness = 125;
                }
            }
            
            last_tick = now;
        } // End of timer if
    } // End of button 11 if
}

static inline uint8_t get_global_brightness() {
    return global_brightness;
}

#endif