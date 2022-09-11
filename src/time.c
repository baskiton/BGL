/*
 * Copyright (c) 2022 Alexander Baskikh
 *
 * MIT License (MIT), http://opensource.org/licenses/MIT
 * Full license can be found in the LICENSE file
 */

#include <stdio.h>

#include <bgl/bgl.h>

#include "internal.h"


BGL_API double bgl_get_time(bgl_instance bgl) {
    return (double)(get_platform_timer_value(bgl) - bgl->timer.offset) / (double)get_platform_timer_freq(bgl);
}

BGL_API void bgl_set_time(bgl_instance bgl, double t) {
    if (t < 0) {
        fprintf(stderr, "set_time: Invalid time %f\n", t);
        return;
    }
    bgl->timer.offset = get_platform_timer_value(bgl) - (uint64_t)(t * (double)get_platform_timer_freq(bgl));
}

BGL_API uint64_t bgl_get_timer(bgl_instance bgl) {
    return get_platform_timer_value(bgl);
}

BGL_API uint64_t bgl_get_timer_freq(bgl_instance bgl) {
    return get_platform_timer_freq(bgl);
}
