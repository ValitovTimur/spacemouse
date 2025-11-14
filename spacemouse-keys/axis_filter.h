#pragma once
#include <stdint.h>
#include <string.h>

#ifndef AXIS_FILTER_MEDIAN3
#define AXIS_FILTER_MEDIAN3 1
#endif
#ifndef AXIS_FILTER_EMA
#define AXIS_FILTER_EMA     1
#endif
#ifndef AXIS_FILTER_SLEW
#define AXIS_FILTER_SLEW    1
#endif

// Default params (tuned for loop ~200–300 Hz)
#ifndef AXIS_FILTER_ALPHA_Q15
// alpha ≈ 0.18 => ~8–10 Hz cut at 200–250 Hz loop
#define AXIS_FILTER_ALPHA_Q15 6000u   // 0..32767 (Q15)
#endif
#ifndef AXIS_FILTER_MAX_STEP
#define AXIS_FILTER_MAX_STEP  5       // max delta per tick, units of axis counts
#endif

typedef struct {
    int16_t m0, m1;     // for median-of-3
    int16_t ema_z1;     // EMA state (Q0)
    int16_t last;       // for slew limiter
    uint8_t inited;
} AxisFilter;

static inline void axisfilter_init(AxisFilter* f, int16_t init_val) {
    memset(f, 0, sizeof(*f));
    f->m0 = f->m1 = f->ema_z1 = f->last = init_val;
    f->inited = 1u;
}

// Median-of-3 helper
static inline int16_t _med3(int16_t a, int16_t b, int16_t c) {
    int16_t lo = (a < b) ? a : b;
    int16_t hi = (a > b) ? a : b;
    if (c < lo) return lo;
    if (c > hi) return hi;
    return c;
}

static inline int16_t axisfilter_median3(AxisFilter* f, int16_t x) {
    int16_t y = _med3(x, f->m0, f->m1);
    f->m1 = f->m0;
    f->m0 = x;
    return y;
}

// EMA: y += alpha*(x - y), alpha in Q15 [0..32767]
static inline int16_t axisfilter_ema_q15(AxisFilter* f, int16_t x, uint16_t alpha_q15) {
    int32_t diff = (int32_t)x - (int32_t)f->ema_z1;
    f->ema_z1 = (int16_t)((int32_t)f->ema_z1 + ((diff * alpha_q15) >> 15));
    return f->ema_z1;
}

static inline int16_t axisfilter_slew(AxisFilter* f, int16_t x, uint16_t max_step) {
    int16_t prev = f->last;
    int16_t d = x - prev;
    if (d >  (int16_t)max_step) x = prev + (int16_t)max_step;
    else if (d < -(int16_t)max_step) x = prev - (int16_t)max_step;
    f->last = x;
    return x;
}

// Full chain
static inline int16_t axisfilter_run(AxisFilter* f, int16_t x) {
    if (!f->inited) axisfilter_init(f, x);
#if AXIS_FILTER_MEDIAN3
    x = axisfilter_median3(f, x);
#endif
#if AXIS_FILTER_EMA
    x = axisfilter_ema_q15(f, x, AXIS_FILTER_ALPHA_Q15);
#endif
#if AXIS_FILTER_SLEW
    x = axisfilter_slew(f, x, AXIS_FILTER_MAX_STEP);
#endif
    return x;
}

// Call when you recenter / reload params
static inline void axisfilter_rebase(AxisFilter* f, int16_t v) {
    f->m0 = f->m1 = f->ema_z1 = f->last = v;
    f->inited = 1u;
}