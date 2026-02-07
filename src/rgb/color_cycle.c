// 1. We create a "memory" for the brightness of each of the 9 LEDs.
// We use 'static' so the Pico remembers these numbers even after the function finishes.
static uint8_t brightness[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};

void ws2812b_color_cycle(uint32_t counter) {
    // Your Button to LED mapping from before
    int led_map[9] = {8, 0, 7, 1, 6, 2, 5, 3, 4};

    // The base colors you want
    // Note: These are R, G, B values. We will apply brightness to these.
    uint8_t colors_r[9] = {255, 255, 0,   0,   255, 0,   0,   255, 255};
    uint8_t colors_g[9] = {255, 255, 255, 0,   0,   0,   255, 255, 255};
    uint8_t colors_b[9] = {255, 0,   0,   255, 0,   255, 0,   0,   255};

    // 2. Check Buttons
    for (int i = 0; i < 9; i++) {
        if (!gpio_get(SW_GPIO[i])) { 
            // Button is pressed: Set brightness to MAX
            brightness[led_map[i]] = 255;
        } else {
            // Button is NOT pressed: Slowly fade out
            // To make it fade SLOWER, change '2' to '1'. 
            // To make it fade FASTER, change '2' to '5'.
            if (brightness[led_map[i]] > 0) {
                brightness[led_map[i]] -= 1; 
            }
        }
    }

    // 3. Send to LEDs
    for (int i = 0; i < 9; i++) {
        // We calculate the color based on the current brightness
        // This math scales the 255 color down to 0
        uint8_t r = (colors_r[i] * brightness[i]) / 255;
        uint8_t g = (colors_g[i] * brightness[i]) / 255;
        uint8_t b = (colors_b[i] * brightness[i]) / 255;
        
        put_pixel(urgb_u32(r, g, b));
    }
}