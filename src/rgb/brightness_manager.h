#ifndef BRIGHTNESS_MANAGER_H
#define BRIGHTNESS_MANAGER_H

#include "pico/stdlib.h"
#include "controller_config.h"

static uint8_t global_brightness = 30;

static inline void handle_brightness_shortcuts() {
    // Hold Button 11 (Index 10)
    if (!gpio_get(SW_GPIO[10])) {
        static uint32_t last_tick = 0;
        uint32_t now = to_ms_since_boot(get_absolute_time());

            if (!gpio_get(SW_GPIO[3])) {
                if (global_brightness >= 5) { // Check if we have at least 5 to subtract
                    global_brightness -= 5;
                } else {
                    global_brightness = 0;   // Otherwise, lock it at 0
                }
            }
            
            if (!gpio_get(SW_GPIO[5])) {
                if (global_brightness < 100) global_brightness += 5;
            }
            last_tick = now;
        }
    }
}

static inline uint8_t get_global_brightness() {
    return global_brightness;
}

#endif