#include "brightness_manager.h"

static uint8_t brightness[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
static uint32_t idle_timer = 1001;
static int scan_idx = 0;
static int scan_dir = 1;

void ws2812b_color_cycle(uint32_t counter) {
    // --- v1.1 SETTINGS ---
    uint8_t MAX_BRIGHTNESS = get_global_brightness();  
    int animation_speed = 60;     // Slightly slower looks better for mirrored moves
    // --------------------

    int led_map[9] = {8, 0, 7, 1, 6, 2, 5, 3, 4};

    uint8_t btn_r[9] = {255, 255, 0,   0,   255, 0,   0,   255, 255};
    uint8_t btn_g[9] = {255, 255, 255, 0,   0,   0,   255, 255, 255};
    uint8_t btn_b[9] = {255, 0,   0,   255, 0,   255, 0,   0,   255};

    bool active = false;

    // 1. Process Buttons and Fading
    for (int i = 0; i < 9; i++) {
        int physical_led = led_map[i];
        if (!gpio_get(SW_GPIO[i])) { 
            brightness[physical_led] = MAX_BRIGHTNESS;
            active = true;
        } else {
            if (brightness[physical_led] > 0) {
                brightness[physical_led] -= 1; 
            }
        }
    }

    // 2. Mirrored Idle Animation (After 10 seconds)
    if (active) {
        idle_timer = 0;
    } else {
        idle_timer++;
        if (idle_timer > 1000) {
            if (idle_timer % animation_speed == 0) {
                
                // Light up the mirrored pair
                // scan_idx will go 0, 1, 2, 3, 4
                brightness[led_map[scan_idx]] = MAX_BRIGHTNESS;      // Left-side moving in
                brightness[led_map[8 - scan_idx]] = MAX_BRIGHTNESS;  // Right-side moving in

                scan_idx += scan_dir;
                
                // Bounce logic (only goes up to 4 since it's mirrored)
                if (scan_idx >= 4) {
                    scan_idx = 4;
                    scan_dir = -1;
                } else if (scan_idx <= 0) {
                    scan_idx = 0;
                    scan_dir = 1;
                }
            }
        }
    }

    // 3. Render
    for (int i = 0; i < 9; i++) {
        int button_index = -1;
        for(int j = 0; j < 9; j++) {
            if(led_map[j] == i) {
                button_index = j;
                break;
            }
        }

        if (button_index != -1) {
            uint8_t r = (btn_r[button_index] * brightness[i]) / 255;
            uint8_t g = (btn_g[button_index] * brightness[i]) / 255;
            uint8_t b = (btn_b[button_index] * brightness[i]) / 255;
            put_pixel(urgb_u32(r, g, b));
        } else {
            put_pixel(0);
        }
    }
}