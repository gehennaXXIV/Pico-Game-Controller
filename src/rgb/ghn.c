/**
 * GHN lighting mode
 *
 * Left knob (encoder 0):
 *   - Turning left  -> blue chase across LEDs 3..8
 *   - Turning right -> blue chase across LEDs 8..3
 *
 * Right knob (encoder 1):
 *   - Turning right -> light red chase across LEDs 1, 0, 19, 18, 17, 16
 *   - Turning left  -> light red chase across LEDs 16, 17, 18, 19, 0, 1
 *
 * Chase behaviour mirrors turbocharger: a soft spotlight (3-LED wide gaussian)
 * travels along the LED sequence, fades out when the knob stops, and snaps
 * back to the start after the idle timeout.
 *
 * Static / reactive layers (evaluated every frame):
 *   LED 10-14  : always-on rainbow (hue spread across the 5 LEDs)
 *   LED 2      : blue  when SW_GPIO[6] is pressed
 *   LED 20-23  : white when SW_GPIO[3,2,1,0] pressed (20->gpio3, 21->gpio2,
 *                                                      22->gpio1, 23->gpio0)
 *   LED 9, 15  : orange when SW_GPIO[4] / SW_GPIO[5] pressed respectively
 *                (nearest unused buttons - adjust SW indices as needed)
 **/

/* ── tuneable constants ─────────────────────────────────────────── */
#define GHN_CLAMP     0.02f
#define GHN_THRESHOLD 0.01f
#define GHN_DECAY     0.001f
#define GHN_VEL       0.02f   /* chase speed in LED-indices per frame  */
#define GHN_FADE      40      /* idle frames before snap-back          */
#define GHN_FADE_VEL  0.025f  /* brightness lost per idle frame        */

/* ── left-knob LED sequence (encoder 0, blue) ───────────────────── */
static const int GHN_L_SEQ[]  = {3, 4, 5, 6, 7, 8};
static const int GHN_L_LEN    = 6;

/* ── right-knob LED sequence (encoder 1, light red) ─────────────── */
static const int GHN_R_SEQ[]  = {1, 0, 19, 18, 17, 16};
static const int GHN_R_LEN    = 6;

/* ── rainbow band LEDs ───────────────────────────────────────────── */
static const int GHN_RAINBOW[] = {10, 11, 12, 13, 14};
static const int GHN_RAINBOW_LEN = 5;

/* ── button-reactive LEDs ───────────────────────────────────────── */
/* led 2  -> SW_GPIO index 6 (GPIO 26) : blue                        */
/* led 20 -> SW_GPIO index 3 (GPIO 8)  : white                       */
/* led 21 -> SW_GPIO index 2 (GPIO 10) : white                       */
/* led 22 -> SW_GPIO index 1 (GPIO 11) : white                       */
/* led 23 -> SW_GPIO index 0 (GPIO 13) : white                       */
/* led 9  -> SW_GPIO index 4 (GPIO 12) : orange                      */
/* led 15 -> SW_GPIO index 5 (GPIO 9)  : orange                      */

/* ── state ──────────────────────────────────────────────────────── */
static uint32_t ghn_prev_enc[ENC_GPIO_SIZE];
static float    ghn_cur_enc[ENC_GPIO_SIZE];
static float    ghn_pos[ENC_GPIO_SIZE];       /* fractional index into seq */
static float    ghn_brightness[ENC_GPIO_SIZE];
static int      ghn_idle[ENC_GPIO_SIZE];
static bool     ghn_init_done = false;

/* ── helpers (safe to redefine if turbocharger.c is excluded) ────── */
#ifndef GHN_HELPERS
#define GHN_HELPERS
static inline float ghn_fclamp(float d, float lo, float hi) {
    return d < lo ? lo : (d > hi ? hi : d);
}
static inline float ghn_fabs(float d) { return d < 0 ? -d : d; }
static inline int   ghn_iclamp(int d, int lo, int hi) {
    return d < lo ? lo : (d > hi ? hi : d);
}
#endif

/* 768-step colour wheel (same as ws2812b_util.c) */
static inline uint32_t ghn_wheel(uint16_t wp) {
    wp %= 768;
    if (wp < 256)       return urgb_u32(255 - wp,       wp,       0);
    else if (wp < 512)  return urgb_u32(0,        255-(wp-256), wp-256);
    else                return urgb_u32(wp-512,         0,  255-(wp-512));
}

/* spotlight strength: falls linearly to 0 at ±1 LED-index away */
static inline float ghn_spot(float pos, float target) {
    return ghn_fclamp(1.0f - ghn_fabs(pos - target), 0.0f, 1.0f);
}

/* ── main function ──────────────────────────────────────────────── */
void ghn(uint32_t counter) {

    /* one-time zero-init (globals are 0 at reset but belt-and-braces) */
    if (!ghn_init_done) {
        for (int i = 0; i < ENC_GPIO_SIZE; i++) {
            ghn_prev_enc[i]  = enc_val[i];
            ghn_cur_enc[i]   = 0.0f;
            ghn_pos[i]       = 0.0f;
            ghn_brightness[i]= 0.0f;
            ghn_idle[i]      = GHN_FADE + 1;
        }
        ghn_init_done = true;
    }

    /* ── 1. update knob state ─────────────────────────────────────── */
    for (int i = 0; i < ENC_GPIO_SIZE; i++) {
        int delta = (int)(enc_val[i] - ghn_prev_enc[i]) * (ENC_REV[i] ? 1 : -1);
        ghn_prev_enc[i] = enc_val[i];

        ghn_cur_enc[i] = ghn_fclamp(
            ghn_cur_enc[i] + (float)delta / ENC_PULSE,
            -GHN_CLAMP, GHN_CLAMP);

        int seq_len = (i == 0) ? GHN_L_LEN : GHN_R_LEN;

        if (ghn_cur_enc[i] < -GHN_THRESHOLD) {
            /* knob turning "left" direction */
            ghn_idle[i] = 0;
            ghn_pos[i] += (i == 1 ? -GHN_VEL : GHN_VEL);
            ghn_brightness[i] = 1.0f;
        } else if (ghn_cur_enc[i] > GHN_THRESHOLD) {
            /* knob turning "right" direction */
            ghn_idle[i] = 0;
            ghn_pos[i] += (i == 1 ? GHN_VEL : -GHN_VEL);
            ghn_brightness[i] = 1.0f;
        } else {
            ghn_idle[i]++;
            if (ghn_idle[i] > GHN_FADE) {
                ghn_pos[i] = 0.0f;
            } else {
                ghn_brightness[i] = ghn_fclamp(
                    ghn_brightness[i] - GHN_FADE_VEL, 0.0f, 1.0f);
            }
        }

        /* wrap position within sequence length */
        while (ghn_pos[i] < 0)        ghn_pos[i] += seq_len;
        while (ghn_pos[i] >= seq_len) ghn_pos[i] -= seq_len;

        /* decay encoder accumulator */
        if      (ghn_cur_enc[i] < -GHN_DECAY) ghn_cur_enc[i] += GHN_DECAY;
        else if (ghn_cur_enc[i] >  GHN_DECAY) ghn_cur_enc[i] -= GHN_DECAY;
    }

    /* ── 2. build per-LED colour buffer ──────────────────────────── */
    uint32_t pixels[WS2812B_LED_SIZE] = {0};

    /* --- left knob: blue spotlight on LEDs 3-8 ------------------- */
    for (int s = 0; s < GHN_L_LEN; s++) {
        float strength = ghn_spot(ghn_pos[0], (float)s) * ghn_brightness[0];
        int   led      = GHN_L_SEQ[s];
        uint8_t b = ghn_iclamp((int)(strength * 255), 0, 255);
        /* accumulate - knob light may overlap button light */
        uint8_t cur_r = (pixels[led] >> 8)  & 0xFF;
        uint8_t cur_g = (pixels[led] >> 16) & 0xFF;
        uint8_t cur_b =  pixels[led]        & 0xFF;
        pixels[led] = urgb_u32(cur_r, cur_g, ghn_iclamp(cur_b + b, 0, 255));
    }

    /* --- right knob: light red spotlight on LEDs 1,0,19,18,17,16 - */
    for (int s = 0; s < GHN_R_LEN; s++) {
        float strength = ghn_spot(ghn_pos[1], (float)s) * ghn_brightness[1];
        int   led      = GHN_R_SEQ[s];
        uint8_t r = ghn_iclamp((int)(strength * 255), 0, 255);
        uint8_t g = 0;
        uint8_t cur_r = (pixels[led] >> 8)  & 0xFF;
        uint8_t cur_g = (pixels[led] >> 16) & 0xFF;
        uint8_t cur_b =  pixels[led]        & 0xFF;
        pixels[led] = urgb_u32(
            ghn_iclamp(cur_r + r, 0, 255),
            ghn_iclamp(cur_g + g, 0, 255),
            ghn_iclamp(cur_b + r, 0, 255));  // ← blue = same as red for magenta
    }

      /* LEDs 10-14: scrolling rainbow, dimmed to 40% */
      #define RAINBOW_DIM 0.4f
      for (int i = 0; i < GHN_RAINBOW_LEN; i++) {
        uint16_t hue = (uint16_t)((counter * 2 + i * (768 / GHN_RAINBOW_LEN)) % 768);
        uint32_t c = ghn_wheel(hue);
        uint8_t r = (uint8_t)(((c >> 8)  & 0xFF) * RAINBOW_DIM);
        uint8_t g = (uint8_t)(((c >> 16) & 0xFF) * RAINBOW_DIM);
        uint8_t b = (uint8_t)( (c        & 0xFF) * RAINBOW_DIM);
        pixels[GHN_RAINBOW[i]] = urgb_u32(r, g, b);
      }

    /* --- button-reactive LEDs ------------------------------------- */

    /* LED 2: blue when SW_GPIO[6] pressed */
    if (!gpio_get(SW_GPIO[6]))
        pixels[2] = urgb_u32(0, 0, 200);

    /* LED 20-23: white when SW_GPIO[3,2,1,0] pressed */
    if (!gpio_get(SW_GPIO[3])) pixels[20] = urgb_u32(200, 200, 200);
    if (!gpio_get(SW_GPIO[2])) pixels[21] = urgb_u32(200, 200, 200);
    if (!gpio_get(SW_GPIO[1])) pixels[22] = urgb_u32(200, 200, 200);
    if (!gpio_get(SW_GPIO[0])) pixels[23] = urgb_u32(200, 200, 200);

    /* LED 9: orange when SW_GPIO[4] pressed */
    if (!gpio_get(SW_GPIO[4])) pixels[9]  = urgb_u32(255, 80, 0);

    /* LED 15: orange when SW_GPIO[5] pressed */
    if (!gpio_get(SW_GPIO[5])) pixels[15] = urgb_u32(255, 80, 0);

    /* ── 3. push all pixels ──────────────────────────────────────── */
    for (int i = 0; i < WS2812B_LED_SIZE; i++) {
        put_pixel(pixels[i]);
    }
}