#include <math.h>
#include "brightness_manager.h"

static uint8_t brightness[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
static uint32_t idle_timer = 1001;

void ws2812b_color_cycle(uint32_t counter) {
    uint8_t MAX_BRIGHTNESS = get_global_brightness();
    
    const float BREATH_SPEED = 0.02f;
    const float BREATH_SPREAD = 0.6f;

    // Maps Button Index to LED Chain Index
    int led_map[9] = {8, 0, 7, 1, 6, 2, 5, 3, 4};
    
    uint8_t btn_r[9] = {255, 255, 0,   0,   255, 0,   0,   255, 255};
    uint8_t btn_g[9] = {255, 255, 255, 0,   0,   0,   255, 255, 255};
    uint8_t btn_b[9] = {255, 0,   0,   255, 0,   255, 0,   0,   255};

    bool active = false;

    // 1. Process Physical Buttons
    for (int i = 0; i < 9; i++) {
        if (!gpio_get(SW_GPIO[i])) { 
            brightness[led_map[i]] = MAX_BRIGHTNESS;
            active = true;
        }
    }

    if (active) idle_timer = 0;
    else if (idle_timer <= 1000) idle_timer++;

    // 2. Rendering Loop
    for (int i = 0; i < 9; i++) {
        int btn_idx = 0;
        for(int j = 0; j < 9; j++) {
            if(led_map[j] == i) {
                btn_idx = j;
                break;
            }
        }

        uint8_t final_b = 0;

        // Run Breathing animation after 5 seconds of no input
        if (idle_timer > 1000) {
            float wave = (sinf(counter * BREATH_SPEED - (btn_idx * BREATH_SPREAD)) + 1.0f) / 2.0f;
            final_b = (uint8_t)(wave * MAX_BRIGHTNESS);
            
            if (brightness[i] > final_b) final_b = brightness[i];
        } else {
            final_b = brightness[i];
        }

        if (brightness[i] > 0) brightness[i] -= 1;

        uint8_t r = (btn_r[btn_idx] * final_b) / 255;
        uint8_t g = (btn_g[btn_idx] * final_b) / 255;
        uint8_t b = (btn_b[btn_idx] * final_b) / 255;
        put_pixel(urgb_u32(r, g, b));
    }
}