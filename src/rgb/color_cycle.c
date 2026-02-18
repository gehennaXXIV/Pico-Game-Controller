#include <math.h>
#include "brightness_manager.h"

static uint8_t brightness[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
static uint32_t idle_timer = 1001;
static int scan_idx = 0;
static int scan_dir = 1;

void ws2812b_color_cycle(uint32_t counter) {
    uint8_t MAX_BRIGHTNESS = get_global_brightness();
    uint8_t mode = get_light_mode();
    
    // Maps Physical Button Index (0-8) to LED Position in Chain
    int led_map[9] = {8, 0, 7, 1, 6, 2, 5, 3, 4};
    
    uint8_t btn_r[9] = {255, 255, 0,   0,   255, 0,   0,   255, 255};
    uint8_t btn_g[9] = {255, 255, 255, 0,   0,   0,   255, 255, 255};
    uint8_t btn_b[9] = {255, 0,   0,   255, 0,   255, 0,   0,   255};

    bool active = false;

    // 1. Physical Button Processing
    for (int i = 0; i < 9; i++) {
        if (!gpio_get(SW_GPIO[i])) { 
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
        // Breathing (Mode 1) is handled in the rendering loop to ensure wave alignment
    }

    // 3. Fading and Rendering
    for (int i = 0; i < 9; i++) {
        // Find which physical Button (j) is at this LED (i)
        int btn_idx = 0;
        for(int j = 0; j < 9; j++) {
            if(led_map[j] == i) {
                btn_idx = j;
                break;
            }
        }

        uint8_t final_b = 0;

        if (mode == 1 && idle_timer > 1000) {
            // BREATHING follows Button Array (btn_idx) 0 -> 1 -> 2 ...
            float wave = (sinf(counter * 0.05f - (btn_idx * 0.6f)) + 1.0f) / 2.0f;
            final_b = (uint8_t)(wave * MAX_BRIGHTNESS);
            
            // Keep button press override
            if (brightness[i] > final_b) final_b = brightness[i];
        } else {
            final_b = brightness[i];
        }

        // Apply background fade
        if (brightness[i] > 0) brightness[i] -= 1;

        // Push to LED
        uint8_t r = (btn_r[btn_idx] * final_b) / 255;
        uint8_t g = (btn_g[btn_idx] * final_b) / 255;
        uint8_t b = (btn_b[btn_idx] * final_b) / 255;
        put_pixel(urgb_u32(r, g, b));
    }
}