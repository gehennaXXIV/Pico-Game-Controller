// Static memory so the Pico remembers the brightness levels between loops
static uint8_t brightness[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};

void ws2812b_color_cycle(uint32_t counter) {
    // 1. THE MAP: Button index -> Physical LED index
    // Button 0 (Pin 15) -> LED 8
    // Button 1 (Pin 14) -> LED 0
    // ...etc
    int led_map[9] = {8, 0, 7, 1, 6, 2, 5, 3, 4};

    // 2. THE COLORS (Defined by Button Index 0-8)
    // These are the R, G, B values for your requested sequence:
    // White, Yellow, Green, Blue, Red, Blue, Green, Yellow, White
    uint8_t btn_r[9] = {255, 255, 0,   0,   255, 0,   0,   255, 255};
    uint8_t btn_g[9] = {255, 255, 255, 0,   0,   0,   255, 255, 255};
    uint8_t btn_b[9] = {255, 0,   0,   255, 0,   255, 0,   0,   255};

    // 3. LOGIC: Check buttons and update brightness
    for (int i = 0; i < 9; i++) {
        int physical_led = led_map[i];

        if (!gpio_get(SW_GPIO[i])) { 
            // Button pressed: Jump to max brightness
            brightness[physical_led] = 255;
        } else {
            // Button released: Fade out slowly (~1 second)
            if (brightness[physical_led] > 0) {
                brightness[physical_led] -= 1; 
            }
        }
    }

    // 4. OUTPUT: Calculate final colors and send to the chain
    // We loop through the physical LEDs (0-8) to send the pixels in order
    for (int i = 0; i < 9; i++) {
        // We need to find which button controls this physical LED to get its color
        int button_index = -1;
        for(int j=0; j<9; j++) {
            if(led_map[j] == i) {
                button_index = j;
                break;
            }
        }

        if (button_index != -1) {
            // Apply the brightness fade to the specific button's color
            uint8_t r = (btn_r[button_index] * brightness[i]) / 255;
            uint8_t g = (btn_g[button_index] * brightness[i]) / 255;
            uint8_t b = (btn_b[button_index] * brightness[i]) / 255;
            
            put_pixel(urgb_u32(r, g, b));
        } else {
            put_pixel(0);
        }
    }
}