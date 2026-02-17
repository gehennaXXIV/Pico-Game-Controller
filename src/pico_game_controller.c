/*
 * Pico Game Controller v1.4
 */
#define PICO_GAME_CONTROLLER_C

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bsp/board.h"
#include "controller_config.h"
#include "encoders.pio.h"
#include "hardware/clocks.h"
#include "hardware/dma.h"
#include "hardware/irq.h"
#include "hardware/pio.h"
#include "pico/multicore.h"
#include "pico/stdlib.h"
#include "tusb.h"
#include "usb_descriptors.h"

// clang-format off
#include "debounce/debounce_include.h"
#include "rgb/rgb_include.h"
// clang-format on

// --- GLOBAL VARIABLES ---
uint8_t global_brightness = 50; 

PIO pio, pio_1;
uint32_t enc_val[ENC_GPIO_SIZE];
uint32_t prev_enc_val[ENC_GPIO_SIZE];
int cur_enc_val[ENC_GPIO_SIZE];

bool sw_prev_raw_val[SW_GPIO_SIZE];
bool sw_cooked_val[SW_GPIO_SIZE];
uint64_t sw_timestamp[SW_GPIO_SIZE];

bool kbm_report;
uint64_t reactive_timeout_timestamp;

void (*ws2812b_mode)();
void (*loop_mode)();
void (*debounce_mode)();
bool joy_mode_check = true;

union {
  struct {
    uint8_t buttons[LED_GPIO_SIZE];
    RGB_t rgb[WS2812B_LED_ZONES];
  } lights;
  uint8_t raw[LED_GPIO_SIZE + WS2812B_LED_ZONES * 3];
} lights_report;

struct report {
  uint16_t buttons;
  uint8_t joy0;
  uint8_t joy1;
} report;

void ws2812b_update(uint32_t counter) {
    ws2812b_mode(counter); 
}

void joy_mode() {
  if (tud_hid_ready()) {
    report.joy0 = 0;
    report.joy1 = 0;
    tud_hid_n_report(0x00, REPORT_ID_JOYSTICK, &report, sizeof(report));
  }
}

void key_mode() {
  if (tud_hid_ready()) {
    uint8_t nkro_report[32] = {0};
    for (int i = 0; i < SW_GPIO_SIZE; i++) {
      if ((report.buttons >> i) & 1) {
        uint8_t bit = SW_KEYCODE[i] % 8;
        uint8_t byte = (SW_KEYCODE[i] / 8) + 1;
        if (SW_KEYCODE[i] >= 240 && SW_KEYCODE[i] <= 247) {
          nkro_report[0] |= (1 << bit);
        } else if (byte > 0 && byte <= 31) {
          nkro_report[byte] |= (1 << bit);
        }
      }
    }
    tud_hid_n_report(0x00, REPORT_ID_KEYBOARD, &nkro_report, sizeof(nkro_report));
  }
}

void update_inputs() {
  report.buttons = 0;
  for (int i = SW_GPIO_SIZE - 1; i >= 0; i--) {
    sw_prev_raw_val[i] = !gpio_get(SW_GPIO[i]);
    report.buttons <<= 1;
    report.buttons |= sw_cooked_val[i];
  }

  // Brightness Control (Hold 11, Press 2 or 7)
  if (!gpio_get(SW_GPIO[10])) {
    if (!gpio_get(SW_GPIO[1])) {
      if (global_brightness > 5) global_brightness -= 5;
      sleep_ms(50);
    }
    if (!gpio_get(SW_GPIO[6])) {
      if (global_brightness < 250) global_brightness += 5;
      sleep_ms(50);
    }
    report.buttons &= ~(1 << 1); 
    report.buttons &= ~(1 << 6);
  }
}

void update_lights() {
  for (int i = 0; i < LED_GPIO_SIZE - 1; i++) {
    if (!gpio_get(SW_GPIO[i])) {
      gpio_put(LED_GPIO[i], 1);
    } else {
      gpio_put(LED_GPIO[i], 0);
    }
  }
}

void dma_handler() {
  uint i = 1;
  int interrupt_channel = 0;
  while ((i & dma_hw->ints0) == 0) {
    i = i << 1;
    ++interrupt_channel;
  }
  dma_hw->ints0 = 1u << interrupt_channel;
}

void core1_entry() {
  uint32_t counter = 0;
  while (1) {
    ws2812b_update(++counter);
    sleep_ms(5);
  }
}

void init() {
  gpio_init(25);
  gpio_set_dir(25, GPIO_OUT);
  gpio_put(25, 1);

  pio_1 = pio1;
  uint offset2 = pio_add_program(pio_1, &ws2812_program);
  ws2812_program_init(pio_1, 0, offset2, WS2812B_GPIO, 800000, false);

  for (int i = 0; i < SW_GPIO_SIZE; i++) {
    gpio_init(SW_GPIO[i]);
    gpio_set_dir(SW_GPIO[i], GPIO_IN);
    gpio_pull_up(SW_GPIO[i]);
  }

  for (int i = 0; i < LED_GPIO_SIZE; i++) {
    gpio_init(LED_GPIO[i]);
    gpio_set_dir(LED_GPIO[i], GPIO_OUT);
  }

  if (!gpio_get(SW_GPIO[10])) {
    loop_mode = &key_mode;
  } else {
    loop_mode = &joy_mode;
  }

  ws2812b_mode = &ws2812b_color_cycle;
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
    update_lights();
  }
  return 0;
}

uint16_t tud_hid_get_report_cb(uint8_t itf, uint8_t report_id, hid_report_type_t report_type, uint8_t* buffer, uint16_t reqlen) {
  return 0;
}

void tud_hid_set_report_cb(uint8_t itf, uint8_t report_id, hid_report_type_t report_type, uint8_t const* buffer, uint16_t bufsize) {
}