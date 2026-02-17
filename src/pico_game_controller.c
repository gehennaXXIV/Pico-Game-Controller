/**
 * Updates input states and stores true state into report.buttons.
 **/
void update_inputs() {
  report.buttons = 0;
  
  // 1. Standard Input Check
  for (int i = SW_GPIO_SIZE - 1; i >= 0; i--) {
    // Note: sw_cooked_val comes from the debounce logic elsewhere
    sw_prev_raw_val[i] = !gpio_get(SW_GPIO[i]);

    report.buttons <<= 1;
    report.buttons |= sw_cooked_val[i];
  }

  // 2. Custom Brightness Controls (v1.4)
  // Check if SW_GPIO 11 (Index 10) is being held
  if (!gpio_get(SW_GPIO[10])) {
    
    // Lower Brightness: SW_GPIO 2 (Index 1)
    if (!gpio_get(SW_GPIO[1])) {
      if (global_brightness > 5) global_brightness -= 5;
      sleep_ms(50); 
    }
    
    // Increase Brightness: SW_GPIO 7 (Index 6)
    if (!gpio_get(SW_GPIO[6])) {
      if (global_brightness < 250) global_brightness += 5;
      sleep_ms(50); 
    }

    // MASKING: Prevent keypresses while adjusting brightness
    report.buttons &= ~(1 << 1);  
    report.buttons &= ~(1 << 6);
  }
} // <--- Check that this brace exists!