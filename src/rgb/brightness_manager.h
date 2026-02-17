#ifndef BRIGHTNESS_MANAGER_H
#define BRIGHTNESS_MANAGER_H

#include "pico/stdlib.h"
#include "controller_config.h"

// The variable lives here now
static uint8_t global_brightness = 50;

static inline void handle_brightness_shortcuts() {
    // Check if Button 11 is held
    if (!gpio_get(SW_GPIO[10])) {
        static uint32_t last_tick = 0;
        uint32_t now = to_ms_since_boot(get_absolute_time());

        // Non-blocking timer: only updates every 50ms
        if (now - last_tick > 50) {
            if (!gpio_get(SW_GPIO[1]) && global_brightness > 5) {
                global_brightness -= 5;
            }
            if (!gpio_get(SW_GPIO[6]) && global_brightness < 250) {
                global_brightness += 5;
            }
            last_tick = now;
        }
    }
}

static inline uint8_t get_global_brightness() {
    return global_brightness;
}

#endif