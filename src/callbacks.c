/*
 * Copyright (c) 2022 Alexander Baskikh
 *
 * MIT License (MIT), http://opensource.org/licenses/MIT
 * Full license can be found in the LICENSE file
 */

#include "internal.h"

BGL_API int bgl_set_window_close_callback(bgl_instance bgl, bgl_close_window_fn callback) {
    if (!bgl->window) {
        fprintf(stderr, "Invalid window\n");
        return false;
    }

    bgl->window->callbacks.close = callback;

    return true;
}

BGL_API int bgl_set_key_callback(bgl_instance bgl, bgl_key_fn callback) {
    if (!bgl->window) {
        fprintf(stderr, "Invalid window\n");
        return false;
    }

    bgl->window->callbacks.key = callback;

    return true;
}
