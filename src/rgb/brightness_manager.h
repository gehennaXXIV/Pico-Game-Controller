#ifndef BRIGHTNESS_MANAGER_H
#define BRIGHTNESS_MANAGER_H

#include "pico/stdlib.h"
#include "controller_config.h"

// Start at 50, but don't let it go below 10 so fade still works
static uint8_t global_brightness = 50;

static inline void handle_brightness_shortcuts() {
    // Hold Button 11 (Index 10)
    if (!gpio_get(SW_GPIO[10])) {
        static uint32_t last_tick = 0;
        uint32_t now = to_ms_since_boot(get_absolute_time());

        if (now - last_tick > 50) {
            // DECREASE: Button 2 (Index 1)
            // Cap at 10 so the fade animation remains visible
            if (!gpio_get(SW_GPIO[1])) {
                if (global_brightness > 20) global_brightness -= 5;
            }
            
            // INCREASE: Button 8 (Index 7)
            // Check if it's below our max limit
            if (!gpio_get(SW_GPIO[7])) {
                if (global_brightness < 250) global_brightness += 5;
            }
            last_tick = now;
        }
    }
}

static inline uint8_t get_global_brightness() {
    return global_brightness;
}

#endif