void ws2812b_color_cycle(uint32_t counter) {
    (void)counter;

    const uint8_t BUTTON_PINS[9] = {
        15, 14, 12, 13, 11, 9, 10, 8, 7
    };

    const RGB_t BUTTON_COLORS[9] = {
        {255, 255, 255}, // white
        {255, 255,   0}, // yellow
        {  0, 255,   0}, // green
        {  0,   0, 255}, // blue
        {255,   0,   0}, // red
        {  0,   0, 255}, // blue
        {  0, 255,   0}, // green
        {255, 255,   0}, // yellow
        {255, 255, 255}  // white
    };

    // clear LEDs
    for (int i = 0; i < WS2812B_LED_SIZE; i++) {
        ws2812b_data[i] = (RGB_t){0, 0, 0};
    }

    // light LED only when its button is pressed
    for (int i = 0; i < 9; i++) {
        if (!gpio_get(BUTTON_PINS[i])) { // active-low
            ws2812b_data[i] = BUTTON_COLORS[i];
        }
    }

    // push data
    for (int i = 0; i < WS2812B_LED_SIZE; i++) {
        put_pixel(urgb_u32(
            ws2812b_data[i].r,
            ws2812b_data[i].g,
            ws2812b_data[i].b
        ));
    }
}
