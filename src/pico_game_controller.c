#define PICO_GAME_CONTROLLER_C

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "bsp/board.h"      // board_init()
#include "controller_config.h"
#include "tusb.h"
#include "usb_descriptors.h"
#include "hardware/gpio.h"
#include "hardware/pio.h"
#include "hardware/dma.h"
#include "pico/stdlib.h"
#include "pico/multicore.h"

PIO pio;
uint32_t enc_val[ENC_GPIO_SIZE];
uint32_t prev_enc_val[ENC_GPIO_SIZE];
int cur_enc_val[ENC_GPIO_SIZE];

bool sw_prev_raw_val[SW_GPIO_SIZE];
bool sw_cooked_val[SW_GPIO_SIZE];
uint64_t sw_timestamp[SW_GPIO_SIZE];

bool kbm_report;
uint64_t reactive_timeout_timestamp;

bool joy_mode_check = true;

struct report {
    uint16_t buttons;
    uint8_t joy0;
    uint8_t joy1;
} report;

// ----- Stub functions -----
void joy_mode(void) {}
void key_mode(void) {}
void debounce_mode(void) {}
void update_inputs(void) {}
void update_lights(void) {}

// Initialize board pins and peripherals
void init() {
    // Onboard LED
    gpio_init(25);
    gpio_set_dir(25, GPIO_OUT);
    gpio_put(25, 1);

    // Encoder initial values
    for (int i = 0; i < ENC_GPIO_SIZE; i++) {
        enc_val[i] = prev_enc_val[i] = cur_enc_val[i] = 0;
    }

    // Button GPIO setup
    for (int i = 0; i < SW_GPIO_SIZE; i++) {
        sw_prev_raw_val[i] = false;
        sw_cooked_val[i] = false;
        sw_timestamp[i] = 0;
        gpio_init(SW_GPIO[i]);
        gpio_set_dir(SW_GPIO[i], GPIO_IN);
        gpio_pull_up(SW_GPIO[i]);
    }

    kbm_report = false;

    // Mode switch (button 0)
    joy_mode_check = gpio_get(SW_GPIO[0]);

    reactive_timeout_timestamp = time_us_64();
}

int main(void) {
    board_init(); // TinyUSB board init
    init();
    tusb_init();

    while (1) {
        tud_task();    // TinyUSB task
        debounce_mode();
        update_inputs();
        if (joy_mode_check) {
            joy_mode();
        } else {
            key_mode();
        }
        update_lights();
    }

    return 0;
}
