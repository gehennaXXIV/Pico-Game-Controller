#define PICO_GAME_CONTROLLER_C
#include <stdio.h>
#include "pico/stdlib.h"
#include "controller_config.h"
#include "pico/multicore.h"
#include "tusb.h"
#include "usb_descriptors.h"
#include "debounce/debounce_include.h"
#include "rgb/rgb_include.h"

// Global Brightness
uint8_t global_brightness = 50; 

// Controller State
struct report {
  uint16_t buttons;
  uint8_t joy0;
  uint8_t joy1;
} report;

bool sw_cooked_val[SW_GPIO_SIZE];
void (*loop_mode)();
void (*ws2812b_mode)();
void (*debounce_mode)();

// Gamepad Detection Fix
void joy_mode() {
  if (tud_hid_ready()) {
    report.joy0 = 127; // Center stick
    report.joy1 = 127;
    tud_hid_n_report(0x00, REPORT_ID_JOYSTICK, &report, sizeof(report));
  }
}

// Keyboard Mode
void key_mode() {
  if (tud_hid_ready()) {
    uint8_t nkro[32] = {0};
    for (int i = 0; i < SW_GPIO_SIZE; i++) {
      if ((report.buttons >> i) & 1) {
        uint8_t bit = SW_KEYCODE[i] % 8;
        uint8_t byte = (SW_KEYCODE[i] / 8) + 1;
        if (SW_KEYCODE[i] >= 240 && SW_KEYCODE[i] <= 247) nkro[0] |= (1 << bit);
        else if (byte > 0 && byte <= 31) nkro[byte] |= (1 << bit);
      }
    }
    tud_hid_n_report(0x00, REPORT_ID_KEYBOARD, &nkro, sizeof(nkro));
  }
}

// Fixed Update Inputs (No Freezing)
void update_inputs() {
  report.buttons = 0;
  for (int i = SW_GPIO_SIZE - 1; i >= 0; i--) {
    report.buttons <<= 1;
    report.buttons |= sw_cooked_val[i];
  }

  // Brightness: Hold Button 11 (Index 10)
  if (!gpio_get(SW_GPIO[10])) {
    static uint32_t last_tick = 0;
    if (board_millis() - last_tick > 50) { // Non-blocking timer
      if (!gpio_get(SW_GPIO[1]) && global_brightness > 5) global_brightness -= 5;
      if (!gpio_get(SW_GPIO[6]) && global_brightness < 250) global_brightness += 5;
      last_tick = board_millis();
    }
    report.buttons &= ~((1 << 1) | (1 << 6)); // Mask keys 2 and 7
  }
}

void core1_entry() {
  uint32_t counter = 0;
  while (1) {
    ws2812b_color_cycle(++counter);
    sleep_ms(5);
  }
}

void init() {
  for (int i = 0; i < SW_GPIO_SIZE; i++) {
    gpio_init(SW_GPIO[i]);
    gpio_set_dir(SW_GPIO[i], GPIO_IN);
    gpio_pull_up(SW_GPIO[i]);
  }
  // Default logic: hold button 11 at boot for Keyboard, otherwise Gamepad
  if (!gpio_get(SW_GPIO[10])) loop_mode = &key_mode;
  else loop_mode = &joy_mode;
  
  debounce_mode = &debounce_eager;
  multicore_launch_core1(core1_entry);
}

int main(void) {
  board_init();
  init();
  tusb_init();
  while (1) {
    tud_task();
    debounce_mode();
    update_inputs();
    loop_mode();
  }
  return 0;
}

// Mandatory Callbacks
uint16_t tud_hid_get_report_cb(uint8_t itf, uint8_t report_id, hid_report_type_t report_type, uint8_t* buffer, uint16_t reqlen) { return 0; }
void tud_hid_set_report_cb(uint8_t itf, uint8_t report_id, hid_report_type_t report_type, uint8_t const* buffer, uint16_t bufsize) {}