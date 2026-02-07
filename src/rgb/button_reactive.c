#include "../controller_config.h"  // <--- ADD THIS LINE
#include "pico/stdlib.h"           // Adds the gpio_get tool

/**
 * Button Reactive Lighting for 9 LEDs
 **/

#define COLOR_WHITE  urgb_u32(255, 255, 255)
#define COLOR_YELLOW urgb_u32(255, 255, 0)
#define COLOR_GREEN  urgb_u32(0, 255, 0)
#define COLOR_BLUE   urgb_u32(0, 0, 255)
#define COLOR_RED    urgb_u32(255, 0, 0)
#define COLOR_OFF    urgb_u32(0, 0, 0)

void button_reactive_lighting(uint32_t unused) {
    uint32_t target_colors[9] = {
        COLOR_WHITE, COLOR_YELLOW, COLOR_GREEN, COLOR_BLUE, 
        COLOR_RED, 
        COLOR_BLUE, COLOR_GREEN, COLOR_YELLOW, COLOR_WHITE
    };

    for (int i = 0; i < 9; i++) {
        // Now BUTTON_GPIO will be recognized
        if (!gpio_get(BUTTON_GPIO[i])) { 
            put_pixel(target_colors[i]);
        } else {
            put_pixel(COLOR_OFF);
        }
    }
}