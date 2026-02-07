void ws2812b_color_cycle(uint32_t counter) {
    // 1. Your requested colors
    uint32_t colors[9] = {
        urgb_u32(255, 255, 255), // White
        urgb_u32(255, 255, 0),   // Yellow
        urgb_u32(0, 255, 0),     // Green
        urgb_u32(0, 0, 255),     // Blue
        urgb_u32(255, 0, 0),     // Red
        urgb_u32(0, 0, 255),     // Blue
        urgb_u32(0, 255, 0),     // Green
        urgb_u32(255, 255, 0),   // Yellow
        urgb_u32(255, 255, 255)  // White
    };

    // 2. THE LED MAP
    // This tells the code: Button 0 controls LED 8, Button 1 controls LED 0, etc.
    // (Note: We use 0-8 for coding instead of 1-9)
    int led_map[9] = {8, 0, 7, 1, 6, 2, 5, 3, 4};

    // 3. Create a temporary list to hold which LEDs should be ON
    uint32_t led_output[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};

    for (int i = 0; i < 9; i++) {
        // If button 'i' is pressed...
        if (!gpio_get(SW_GPIO[i])) { 
            // ...put that button's color into the correct LED position
            led_output[led_map[i]] = colors[i];
        }
    }

    // 4. Send the pixels to the chain in the physical order (0 to 8)
    for (int i = 0; i < 9; i++) {
        put_pixel(led_output[i]);
    }
}