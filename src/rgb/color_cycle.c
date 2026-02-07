void ws2812b_color_cycle(uint32_t counter) {
    (void)counter;

    const RGB_t COLORS[9] = {
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

    for (int i = 0; i < WS2812B_LED_SIZE; i++) {
        RGB_t c = {0, 0, 0};

        // buttons 0–8 map to LEDs 0–8
        if (i < 9 && ((report.buttons >> i) & 1)) {
            c = COLORS[i];
        }

        put_pixel(urgb_u32(c.r, c.g, c.b));
    }
}
