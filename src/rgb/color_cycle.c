static uint8_t brightness[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};

void ws2812b_color_cycle(uint32_t counter) {
    // --- ADJUST BRIGHTNESS HERE ---
    // 255 = 100% (Blinding)
    // 128 = 50%  (Comfortable)
    // 64  = 25%  (Dim/Night mode)
    uint8_t MAX_BRIGHTNESS = 100; 
    // ------------------------------

    int led_map[9] = {8, 0, 7, 1, 6, 2, 5, 3, 4};

    uint8_t btn_r[9] = {255, 255, 0,   0,   255, 0,   0,   255, 255};
    uint8_t btn_g[9] = {255, 255, 255, 0,   0,   0,   255, 255, 255};
    uint8_t btn_b[9] = {255, 0,   0,   255, 0,   255, 0,   0,   255};

    for (int i = 0; i < 9; i++) {
        int physical_led = led_map[i];

        if (!gpio_get(SW_GPIO[i])) { 
            // When pressed, jump to your MAX instead of 255
            brightness[physical_led] = MAX_BRIGHTNESS;
        } else {
            // Fade logic (stays at -= 1 for the 1-second feel)
            if (brightness[physical_led] > 0) {
                brightness[physical_led] -= 1; 
            }
        }
    }

    for (int i = 0; i < 9; i++) {
        int button_index = -1;
        for(int j = 0; j < 9; j++) {
            if(led_map[j] == i) {
                button_index = j;
                break;
            }
        }

        if (button_index != -1) {
            // We now divide by MAX_BRIGHTNESS to keep the colors accurate
            // If brightness is 0, the result is 0 (off).
            uint8_t r = 0, g = 0, b = 0;
            if (brightness[i] > 0) {
                r = (btn_r[button_index] * brightness[i]) / 255;
                g = (btn_g[button_index] * brightness[i]) / 255;
                b = (btn_b[button_index] * brightness[i]) / 255;
            }
            
            put_pixel(urgb_u32(r, g, b));
        } else {
            put_pixel(0);
        }
    }
}