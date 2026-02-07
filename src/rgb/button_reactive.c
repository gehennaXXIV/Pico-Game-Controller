#include "pico/stdlib.h"

// This tells the compiler the button list is defined in another file
extern const uint8_t BUTTON_GPIO[];

/**
 * Button Reactive Lighting
 **/
void button_reactive_lighting(uint32_t unused) {
    // Your requested colors
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
        // gpio_get reads the pin. ! means "is pressed"
        if (!gpio_get(BUTTON_GPIO[i])) { 
            put_pixel(target_colors[i]);
        } else {
            put_pixel(0); // Off
        }
    }
}