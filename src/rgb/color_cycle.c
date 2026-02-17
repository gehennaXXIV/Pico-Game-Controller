static uint8_t brightness[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
static uint32_t idle_timer = 1001;
static int scan_idx = 0;
static int scan_dir = 1;

extern uint8_t global_brightness; // Link to the variable

void ws2812b_color_cycle(uint32_t counter) {
    uint8_t MAX_BRIGHTNESS = global_brightness; // Use the global variable
    int animation_speed = 60;
    int led_map[9] = {8, 0, 7, 1, 6, 2, 5, 3, 4};
    uint8_t btn_r[9] = {255, 255, 0, 0, 255, 0, 0, 255, 255};
    uint8_t btn_g[9] = {255, 255, 255, 0, 0, 0, 255, 255, 255};
    uint8_t btn_b[9] = {255, 0, 0, 255, 0, 255, 0, 0, 255};

    bool active = false;
    for (int i = 0; i < 9; i++) {
        if (!gpio_get(SW_GPIO[i])) { 
            brightness[led_map[i]] = MAX_BRIGHTNESS;
            active = true;
        } else if (brightness[led_map[i]] > 0) {
            brightness[led_map[i]] -= 1; 
        }
    }

    if (active) {
        idle_timer = 0;
    } else {
        idle_timer++;
        if (idle_timer > 1000 && idle_timer % animation_speed == 0) {
            brightness[led_map[scan_idx]] = MAX_BRIGHTNESS;
            brightness[led_map[8 - scan_idx]] = MAX_BRIGHTNESS;
            scan_idx += scan_dir;
            if (scan_idx >= 4) { scan_idx = 4; scan_dir = -1; }
            else if (scan_idx <= 0) { scan_idx = 0; scan_dir = 1; }
        }
    }

    for (int i = 0; i < 9; i++) {
        int button_index = -1;
        for(int j = 0; j < 9; j++) if(led_map[j] == i) { button_index = j; break; }
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