/**
 * Button Reactive Lighting
 * Colors: White, Yellow, Green, Blue, Red, Blue, Green, Yellow, White
 **/

// Define colors using the urgb_u32 helper from ws2812b_util.c
#define COLOR_WHITE  urgb_u32(255, 255, 255)
#define COLOR_YELLOW urgb_u32(255, 255, 0)
#define COLOR_GREEN  urgb_u32(0, 255, 0)
#define COLOR_BLUE   urgb_u32(0, 0, 255)
#define COLOR_RED    urgb_u32(255, 0, 0)
#define COLOR_OFF    urgb_u32(0, 0, 0)

void button_reactive_lighting(uint32_t unused) {
    // Array mapping your 9 LEDs to specific colors
    uint32_t target_colors[9] = {
        COLOR_WHITE, COLOR_YELLOW, COLOR_GREEN, COLOR_BLUE, 
        COLOR_RED, 
        COLOR_BLUE, COLOR_GREEN, COLOR_YELLOW, COLOR_WHITE
    };

    // Assuming the "pressed" state is handled via GPIO or a global button array
    // You may need to adjust the logic below to match your specific button-reading function
    for (int i = 0; i < WS2812B_LED_SIZE; i++) {
        // Replace 'is_button_pressed(i)' with your actual button check logic
        if (is_button_pressed(i)) { 
            put_pixel(target_colors[i % 9]);
        } else {
            put_pixel(COLOR_OFF);
        }
    }
}