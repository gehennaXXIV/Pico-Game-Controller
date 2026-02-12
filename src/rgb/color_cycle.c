#include "rgb_include.h"

// Helper function to keep numbers between 0 and 255
int i_clamp(int d, int min, int max) {
  const int t = d < min ? min : d;
  return t > max ? max : t;
}

// Memory for the fade and idle animation
static uint8_t brightness[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
static uint32_t idle_counter = 0;
static int scanner_pos = 0;
static int scanner_dir = 1; 
static uint32_t animation_timer = 0;

void ws2812b_color_cycle(uint32_t counter) {
    uint8_t MAX_BRIGHTNESS = 100; 
    int led_map[9] = {8, 0, 7, 1, 6, 2, 5, 3, 4};

    // Button colors (RGB)
    uint8_t btn_r[9] = {255, 255, 0,   0,   255, 0,   0,   255, 255};
    uint8_t btn_g[9] = {255, 255, 255, 0,   0,   0,   255, 255, 255};
    uint8_t btn_b[9] = {255, 0,   0,   255, 0,   255, 0,   0,   255};

    bool activity = false;

    // 1. Check Buttons & Handle Fade
    for (int i = 0; i < 9; i++) {
        int physical_led = led_map[i];
        if (!gpio_get(SW_GPIO[i])) { 
            brightness[physical_led] = MAX_BRIGHTNESS;
            activity = true;
        } else {
            if (brightness[physical_led] > 0) {
                brightness[physical_led] -= 1; 
            }
        }
    }

    // 2. Idle Timer Logic
    if (activity) {
        idle_counter = 0;
        animation_timer = 0;
        scanner_pos = 0;
        scanner_dir = 1;
    } else {
        idle_counter++; 
    }

    // 3. Animation Trigger (2000 loops = 10 seconds)
    if (idle_counter > 2000) {
        animation_timer++;
        if (animation_timer >= 20) {
            animation_timer = 0;
            brightness[led_map[scanner_pos]] = MAX_BRIGHTNESS;
            scanner_pos += scanner_dir;
            if (scanner_pos >= 8) {
                scanner_pos = 8;
                scanner_dir = -1;
            } else if (scanner_pos <= 0) {
                scanner_pos = 0;
                scanner_dir = 1;
            }
        }
    }

    // 4. Output to LED Strip
    for (int i = 0; i < 9; i++) {
        int button_index = -1;
        for(int j = 0; j < 9; j++) {
            if(led_map[j] == i) {
                button_index = j;
                break;
            }
        }

        if (button_index != -1) {
            uint8_t r = i_clamp((btn_r[button_index] * brightness[i]) / 255, 0, 255);
            uint8_t g = i_clamp((btn_g[button_index] * brightness[i]) / 255, 0, 255);
            uint8_t b = i_clamp((btn_b[button_index] * brightness[i]) / 255, 0, 255);
            put_pixel(urgb_u32(r, g, b));
        } else {
            put_pixel(0);
        }
    }
}