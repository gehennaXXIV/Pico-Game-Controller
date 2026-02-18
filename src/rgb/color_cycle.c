#include <math.h>
#include <stdlib.h> // For rand()
#include "brightness_manager.h"

static uint8_t brightness[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
static uint32_t idle_timer = 1001;
static int scan_idx = 0;
static int scan_dir = 1;

void ws2812b_color_cycle(uint32_t counter) {
    uint8_t MAX_BRIGHTNESS = get_global_brightness();
    uint8_t current_mode = get_light_mode();
    int led_map[9] = {8, 0, 7, 1, 6, 2, 5, 3, 4};
    
    uint8_t btn_r[9] = {255, 255, 0,   0,   255, 0,   0,   255, 255};
    uint8_t btn_g[9] = {255, 255, 255, 0,   0,   0,   255, 255, 255};
    uint8_t btn_b[9] = {255, 0,   0,   255, 0,   255, 0,   0,   255};

    bool active = false;

    // 1. Physical Button Input (Always priority)
    for (int i = 0; i < 9; i++) {
        if (!gpio_get(SW_GPIO[i])) { 
            brightness[led_map[i]] = MAX_BRIGHTNESS;
            active = true;
        }
    }

    // 2. Idle Animations
    if (active) {
        idle_timer = 0;
    } else {
        idle_timer++;
        if (idle_timer > 1000) {
            // MODE 0: PING PONG
            if (current_mode == 0) {
                if (idle_timer % 60 == 0) {
                    brightness[led_map[scan_idx]] = MAX_BRIGHTNESS;
                    brightness[led_map[8 - scan_idx]] = MAX_BRIGHTNESS;
                    scan_idx += scan_dir;
                    if (scan_idx >= 4) { scan_idx = 4; scan_dir = -1; }
                    else if (scan_idx <= 0) { scan_idx = 0; scan_dir = 1; }
                }
            } 
            // MODE 1: BREATHING (Left to Right)
            else if (current_mode == 1) {
                for (int i = 0; i < 9; i++) {
                    float offset = i * 0.5f;
                    float wave = (sinf(counter * 0.05f - offset) + 1.0f) / 2.0f;
                    brightness[led_map[i]] = (uint8_t)(wave * MAX_BRIGHTNESS);
                }
            }
            // MODE 2: DROPLETS
            else if (current_mode == 2) {
                // Randomly pick an LED to "drip" every ~150ms
                if (counter % 30 == 0) {
                    int drip = rand() % 9;
                    brightness[drip] = MAX_BRIGHTNESS;
                }
            }
        }
    }

    // 3. Global Fading & Rendering
    for (int i = 0; i < 9; i++) {
        // Fade logic only applies to Ping-Pong, Droplets, or active button presses
        // Breathing manages its own brightness via the sine wave
        if (current_mode != 1) {
            if (brightness[i] > 0) brightness[i] -= 1;
        }

        uint8_t r = (btn_r[i] * brightness[i]) / 255;
        uint8_t g = (btn_g[i] * brightness[i]) / 255;
        uint8_t b = (btn_b[i] * brightness[i]) / 255;
        put_pixel(urgb_u32(r, g, b));
    }
}