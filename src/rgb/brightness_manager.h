#ifndef BRIGHTNESS_MANAGER_H
#define BRIGHTNESS_MANAGER_H

#include "pico/stdlib.h"
#include "controller_config.h"

static uint8_t global_brightness = 30;
static uint8_t light_mode = 0; // 0=Ping-Pong, 1=Breathing, 2=Droplets

static inline void handle_brightness_shortcuts() {
    if (!gpio_get(SW_GPIO[9])) { 
        static uint32_t last_tick = 0;
        uint32_t now = to_ms_since_boot(get_absolute_time());

        if (now - last_tick > 50) {
            if (!gpio_get(SW_GPIO[0])) light_mode = 1; 
            if (!gpio_get(SW_GPIO[1])) light_mode = 0; 
            if (!gpio_get(SW_GPIO[2])) light_mode = 2; 

            if (!gpio_get(SW_GPIO[3])) {
                if (global_brightness >= 5) global_brightness -= 5;
                else global_brightness = 0;
            }
            if (!gpio_get(SW_GPIO[5])) {
                if (global_brightness <= 120) global_brightness += 5;
                else global_brightness = 125;
            }
            last_tick = now;
        }
    }
}

static inline uint8_t get_global_brightness() { return global_brightness; }
static inline uint8_t get_light_mode() { return light_mode; }

#endif