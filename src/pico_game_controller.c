/*
 * Pico Game Controller - RP2040 Zero
 * LED / WS2812B removed
 */
#define PICO_GAME_CONTROLLER_C

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
// clang-format on

PIO pio;
uint32_t enc_val[ENC_GPIO_SIZE];
uint32_t prev_enc_val[ENC_GPIO_SIZE];
int cur_enc_val[ENC_GPIO_SIZE];

bool sw_prev_raw_val[SW_GPIO_SIZE];
bool sw_cooked_val[SW_GPIO_SIZE];
uint64_t sw_timestamp[SW_GPIO_SIZE];

bool kbm_report;

void (*loop_mode)();
void (*debounce_mode)();
bool joy_mode_check = true;

struct report {
    uint16_t buttons;
    uint8_t joy0;
    uint8_t joy1;
} report;

/**
 * Gamepad Mode
 **/
void joy_mode() {
    if (tud_hid_ready()) {
        for (int i = 0; i < ENC_GPIO_SIZE; i++) {
            cur_enc_val[i] += ((ENC_REV[i] ? 1 : -1) * (enc_val[i] - prev_enc_val[i]));
            while (cur_enc_val[i] < 0) cur_enc_val[i] += ENC_PULSE;
            cur_enc_val[i] %= ENC_PULSE;
            prev_enc_val[i] = enc_val[i];
        }

        report.joy0 = ((double)cur_enc_val[0] / ENC_PULSE) * (UINT8_MAX + 1);
        report.joy1 = ((double)cur_enc_val[1] / ENC_PULSE) * (UINT8_MAX + 1);

        tud_hid_n_report(0x00, REPORT_ID_JOYSTICK, &report, sizeof(report));
    }
}

/**
 * Keyboard / Mouse Mode
 **/
void key_mode() {
    if (tud_hid_ready()) {
        if (kbm_report) {
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
        } else {
            int delta[ENC_GPIO_SIZE] = {0};
            for (int i = 0; i < ENC_GPIO_SIZE; i++) {
                delta[i] = (enc_val[i] - prev_enc_val[i]) * (ENC_REV[i] ? 1 : -1);
                prev_enc_val[i] = enc_val[i];
            }
            tud_hid_mouse_report(REPORT_ID_MOUSE, 0x00, delta[0] * MOUSE_SENS, delta[1] * MOUSE_SENS, 0, 0);
        }
        kbm_report = !kbm_report;
    }
}

/**
 * Update inputs
 **/
void update_inputs() {
    report.buttons = 0;
    for (int i = SW_GPIO_SIZE - 1; i >= 0; i--) {
        sw_prev_raw_val[i] = !gpio_get(SW_GPIO[i]);
        report.buttons <<= 1;
        report.buttons |= sw_cooked_val[i];
    }
}

/**
 * DMA Encoder Handler
 **/
void dma_handler() {
    uint i = 1;
    int interrupt_channel = 0;
    while ((i & dma_hw->ints0) == 0) {
        i <<= 1;
        ++interrupt_channel;
    }
    dma_hw->ints0 = 1u << interrupt_channel;
    if (interrupt_channel < 4) {
        dma_channel_set_read_addr(interrupt_channel, &pio->rxf[interrupt_channel], true);
    }
}

/**
 * Initialize Board Pins
 **/
void init() {
    gpio_init(25);
    gpio_set_dir(25, GPIO_OUT);
    gpio_put(25, 1);

    pio = pio0;
    uint offset = pio_add_program(pio, &encoders_program);

    for (int i = 0; i < ENC_GPIO_SIZE; i++) {
        enc_val[i] = prev_enc_val[i] = cur_enc_val[i] = 0;
        encoders_program_init(pio, i, offset, ENC_GPIO[i], ENC_DEBOUNCE);

        dma_channel_config c = dma_channel_get_default_config(i);
        channel_config_set_read_increment(&c, false);
        channel_config_set_write_increment(&c, false);
        channel_config_set_dreq(&c, pio_get_dreq(pio, i, false));

        dma_channel_configure(i, &c, &enc_val[i], &pio->rxf[i], 0x10, true);
        irq_set_exclusive_handler(DMA_IRQ_0, dma_handler);
        irq_set_enabled(DMA_IRQ_0, true);
        dma_channel_set_irq0_enabled(i, true);
    }

    for (int i = 0; i < SW_GPIO_SIZE; i++) {
        sw_prev_raw_val[i] = false;
        sw_cooked_val[i] = false;
        sw_timestamp[i] = 0;
        gpio_init(SW_GPIO[i]);
        gpio_set_function(SW_GPIO[i], GPIO_FUNC_SIO);
        gpio_set_dir(SW_GPIO[i], GPIO_IN);
        gpio_pull_up(SW_GPIO[i]);
    }

    kbm_report = false;

    if (!gpio_get(SW_GPIO[0])) {
        loop_mode = &key_mode;
        joy_mode_check = false;
    } else {
        loop_mode = &joy_mode;
        joy_mode_check = true;
    }

    debounce_mode = &debounce_eager;
}

/**
 * Main Loop
 **/
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

/**
 * GET_REPORT callback
 **/
uint16_t tud_hid_get_report_cb(uint8_t itf, uint8_t report_id,
                               hid_report_type_t report_type, uint8_t* buffer,
                               uint16_t reqlen) {
    (void)itf;
    (void)report_id;
    (void)report_type;
    (void)buffer;
    (void)reqlen;
    return 0;
}
