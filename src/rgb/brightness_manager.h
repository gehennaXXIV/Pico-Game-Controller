#ifndef BRIGHTNESS_MANAGER_H
#define BRIGHTNESS_MANAGER_H

#include "pico/stdlib.h"
#include "controller_config.h"

static uint8_t global_brightness = 30;

static inline void handle_brightness_shortcuts() {
    static uint32_t last_tick = 0;
    uint32_t now = to_ms_since_boot(get_absolute_time());

    // Only check every 50ms
    if (now - last_tick > 50) {
        // HOLD BUTTON 11 (Index 10) to adjust brightness
        if (!gpio_get(SW_GPIO[10])) {
            // Button 4 (Index 3) Decrease
            if (!gpio_get(SW_GPIO[3]) && global_brightness >= 5) { 
                global_brightness -= 5; 
            }
            // Button 6 (Index 5) Increase
            if (!gpio_get(SW_GPIO[5]) && global_brightness <= 120) { 
                global_brightness += 5; 
            }
        }
        last_tick = now;
    }
}

static inline uint8_t get_global_brightness() { return global_brightness; }

#endif