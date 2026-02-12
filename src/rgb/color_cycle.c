static uint8_t brightness[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
static uint32_t idle_counter = 0;
static int scanner_pos = 0;
static int scanner_dir = 1; 
static uint32_t animation_timer = 0;

void ws2812b_color_cycle(uint32_t counter) {
    uint8_t MAX_BRIGHTNESS = 100; 
    int led_map[9] = {8, 0, 7, 1, 6, 2, 5, 3, 4};

    uint8_t btn_r[9] = {255, 255, 0,   0,   255, 0,   0,   255, 255};
    uint8_t btn_g[9] = {255, 255, 255, 0,   0,   0,   255, 255, 255};
    uint8_t btn_b[9] = {255, 0,   0,   255, 0,   255, 0,   0,   255};

    bool activity = false;

    // 1. Check Physical Buttons
    for (int i = 0; i < 9; i++) {
        int physical_led = led_map[i];
        if (!gpio_get(SW_GPIO[i])) { 
            brightness[physical_led] = MAX_BRIGHTNESS;
            activity = true;
        } else {
            // Constant Fade logic for all LEDs
            if (brightness[physical_led] > 0) {
                brightness[physical_led] -= 1; 
            }
        }
    }

    // 2. Idle Logic
    if (activity) {
        idle_counter = 0;
        animation_timer = 0;
        // Optional: Reset scanner to start position for next time
        scanner_pos = 0;
        scanner_dir = 1;
    } else {
        idle_counter++; 
    }

    // 3. Trigger Animation after 5 seconds (1000 loops)
    if (idle_counter > 1000) {
        animation_timer++;
        
        // Speed of the "bounce": move every 100ms (20 loops)
        // Change 20 to a higher number to make the animation slower
        if (animation_timer >= 20) {
            animation_timer = 0;
            
            // "Light up" the current position in the sequence
            brightness[led_map[scanner_pos]] = MAX_BRIGHTNESS;

            // Step the scanner
            scanner_pos += scanner_dir;

            // Bounce logic
            if (scanner_pos >= 8) {
                scanner_pos = 8;
                scanner_dir = -1;
            } else if (scanner_pos <= 0) {
                scanner_pos = 0;
                scanner_dir = 1;
            }
        }
    }

    // 4. Send to LED Strip
    for (int i = 0; i < 9; i++) {
        int button_index = -1;
        for(int j = 0; j < 9; j++) {
            if(led_map[j] == i) {
                button_index = j;
                break;
            }
        }

        if (button_index != -1) {
            uint8_t r = (btn_r[button_index] * brightness[i]) / 255;