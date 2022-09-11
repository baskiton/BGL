/*
 * Copyright (c) 2022 Alexander Baskikh
 *
 * MIT License (MIT), http://opensource.org/licenses/MIT
 * Full license can be found in the LICENSE file
 */

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <unistd.h>
#include <time.h>

#include <bgl/bgl.h>

#include "internal.h"


#define S2NS ((uint64_t)1e9)


void init_platform_timer(bgl_instance bgl) {
    bgl->timer.freq = S2NS;

#if defined(_POSIX_MONOTONIC_CLOCK)
    struct timespec ts;
    if (!clock_gettime(CLOCK_MONOTONIC_RAW, &ts))
        bgl->timer.id = CLOCK_MONOTONIC_RAW;
    else if (!clock_gettime(CLOCK_MONOTONIC, &ts))
        bgl->timer.id = CLOCK_MONOTONIC;
#endif
        bgl->timer.id = CLOCK_REALTIME;

    bgl->timer.offset = (uint64_t)ts.tv_sec * bgl->timer.freq + (uint64_t)ts.tv_nsec;
}

uint64_t get_platform_timer_value(bgl_instance bgl) {
    struct timespec ts;
    clock_gettime(bgl->timer.id, &ts);
    return (uint64_t)ts.tv_sec * bgl->timer.freq + (uint64_t)ts.tv_nsec;
}

uint64_t get_platform_timer_freq(bgl_instance bgl) {
    return bgl->timer.freq;
}
