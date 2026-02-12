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

    // 1. Check Buttons
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
        
        // Move every 20 loops (100ms)
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
            uint8_t r = (btn_r[button_index] * brightness[i]) / 25