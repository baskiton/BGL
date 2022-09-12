/*
 * Copyright (c) 2022 Alexander Baskikh
 *
 * MIT License (MIT), http://opensource.org/licenses/MIT
 * Full license can be found in the LICENSE file
 */

#include <unistd.h>
#include <time.h>

#include <bgl/bgl.h>

#include "internal.h"


void init_platform_timer(bgl_instance bgl) {
}

uint64_t get_platform_timer_value(bgl_instance bgl) {
    return 0;
}

uint64_t get_platform_timer_freq(bgl_instance bgl) {
    return bgl->timer.freq;
}
