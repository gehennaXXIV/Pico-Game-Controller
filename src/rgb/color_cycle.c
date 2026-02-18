#include <math.h>
#include <stdlib.h>
#include "brightness_manager.h"

static uint8_t brightness[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
static uint32_t idle_timer = 1001;
static int scan_idx = 0;
static int scan_dir = 1;

void ws2812b_color_cycle(uint32_t counter) {
    uint8_t MAX_BRIGHTNESS = get_global_brightness();
    uint8_t mode = get_light_mode();
    
    // Physical layout map: Maps Button Index -> LED Index
    int led_map[9] = {8, 0, 7, 1, 6, 2, 5, 3, 4};
    
    // Corrected Colors assigned to physical BUTTONS 0-8
    uint8_t btn_r[9] = {255, 255, 0,   0,   255, 0,   0,   255, 255};
    uint8_t btn_g[9] = {255, 255, 255, 0,   0,   0,   255, 255, 255};
    uint8_t btn_b[9] = {255, 0,   0,   255, 0,   255, 0,   0,   255};

    bool active = false;

    // 1. Button Input & Brightness Assignment
    for (int i = 0; i < 9; i++) {
        if (!gpio_get(SW_GPIO[i])) { 
            // We apply the color of button [i] to the mapped LED
            brightness[led_map[i]] = MAX_BRIGHTNESS;
            active = true;
        }
    }

    if (active) {
        idle_timer = 0;
    } else {
        idle_timer++;
    }

    // 2. Idle Animation Logic
    if (idle_timer > 1000) {
        if (mode == 0) { // Ping-Pong
            if (idle_timer % 60 == 0) {
                brightness[led_map[scan_idx]] = MAX_BRIGHTNESS;
                brightness[led_map[8 - scan_idx]] = MAX_BRIGHTNESS;
                scan_idx += scan_dir;
                if (scan_idx >= 4) { scan_idx = 4; scan_dir = -1; }
                else if (scan_idx <= 0) { scan_idx = 0; scan_dir = 1; }
            }
        } 
        else if (mode == 2) { // Droplets (Multiple drops)
            if (counter % 15 == 0) { // Faster drop rate
                brightness[rand() % 9] = MAX_BRIGHTNESS;
                if (rand() % 2 == 0) brightness[rand() % 9] = MAX_BRIGHTNESS; // Chance for 2nd drop
            }
        }
    }

    // 3. Fading and Rendering
    for (int i = 0; i < 9; i++) {
        uint8_t final_b = 0;

        if (mode == 1 && idle_timer > 1000) {
            // BREATHING: Array order 0 -> 1 -> 2 ... -> 8
            // Use i directly for the offset to follow array order
            float wave = (sinf(counter * 0.05f - (i * 0.6f)) + 1.0f) / 2.0f;
            final_b = (uint8_t)(wave * MAX_BRIGHTNESS);
            
            // Keep button press visibility
            if (brightness[i] > final_b) final_b = brightness[i];
        } else {
            final_b = brightness[i];
        }

        // Apply fade to the background buffer
        if (brightness[i] > 0) brightness[i] -= 1;

        // --- COLOR CORRECTION STEP ---
        // We need to find which button [j] corresponds to LED [i] 
        // to use the correct color array values.
        int button_ref = 0;
        for(int j = 0; j < 9; j++) {
            if(led_map[j] == i) {
                button_ref = j;
                break;
            }
        }

        uint8_t r = (btn_r[button_ref] * final_b) / 255;
        uint8_t g = (btn_g[button_ref] * final_b) / 255;
        uint8_t b = (btn_b[button_ref] * final_b) / 255;
        put_pixel(urgb_u32(r, g, b));
    }
}