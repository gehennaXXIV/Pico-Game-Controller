#include "pico/stdlib.h"
#include "../controller_config.h"

/**
 * Button Reactive Lighting
 * Colors: White, Yellow, Green, Blue, Red, Blue, Green, Yellow, White
 **/

void button_reactive_lighting(uint32_t unused) {
    // These match the 9 LEDs in your chain
    uint32_t target_colors[9] = {
        urgb_u32(255, 255, 255), // White
        urgb_u32(255, 255, 0),   // Yellow
        urgb_u32(0, 255, 0),     // Green
        urgb_u32(0, 0, 255),     // Blue
        urgb_u32(255, 0, 0),     // Red
        urgb_u32(0, 0, 255),     // Blue
        urgb_u32(0, 255, 0),     // Green
        urgb_u32(255, 255, 0),   // Yellow
        urgb_u32(255, 255, 255)  // White
    };

    for (int i = 0; i < 9; i++) {
        // gpio_get checks the pin. ! means "if pressed (0)"
        if (!gpio_get(BUTTON_GPIO[i])) { 
            put_pixel(target_colors[i]);
        } else {
            put_pixel(0); // Off
        }
    }
}