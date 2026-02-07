void ws2812b_color_cycle(uint32_t counter) {
    // Your 9 colors in order
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

    for (int i = 0; i < 9; i++) {
        // We use !gpio_get because buttons are 0 when pressed
        if (!gpio_get(SW_GPIO[i])) { 
            put_pixel(colors[i]);
        } else {
            put_pixel(0); // Off when not pressed
        }
    }
}