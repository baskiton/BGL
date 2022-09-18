/*
 * Copyright (c) 2022 Alexander Baskikh
 *
 * MIT License (MIT), http://opensource.org/licenses/MIT
 * Full license can be found in the LICENSE file
 */

#include <errno.h>
#include <float.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include <stdlib.h>
#include <bgl/bgl.h>

#include "internal.h"


BGL_API int bgl_create_window(bgl_instance bgl, int width, int height, const char *title) {
    if (bgl->window) {
        fputs("Window already exist\n", stderr);
        return false;
    }

    if (width <= 0 || height <= 0) {
        fprintf(stderr, "Invalid window size: %dx%d\n", width, height);
        return false;
    }

    if (!(bgl->window = calloc(1, sizeof(*bgl->window)))) {
        fprintf(stderr, "Failed to create window: %s\n", strerror(errno));
        return false;
    }

    bgl_window_cfg w_cfg = bgl->default_cfgs.window;
    bgl_fb_cfg fb_cfg = bgl->default_cfgs.framebuffer;
    bgl_render_cfg rndr_cfg = bgl->default_cfgs.render;

    w_cfg.width = width;
    w_cfg.height = height;
    w_cfg.title = title ? : "BGL-Window";

    bgl->window->resizable = w_cfg.resizable;

    if (!create_platform_window(bgl, &w_cfg, &fb_cfg, &rndr_cfg))
        bgl_destroy_window(bgl);

    return (bool)bgl->window;
}

BGL_API void bgl_destroy_window(bgl_instance bgl) {
    if (!bgl->window)
        return;

    destroy_platform_window(bgl);

    free(bgl->window);
    bgl->window = NULL;
}

BGL_API int bgl_window_should_close(bgl_instance bgl) {
    if (!bgl->window) {
        fprintf(stderr, "Invalid window\n");
        return false;
    }

    return bgl->window->should_close;
}

BGL_API void bgl_set_window_should_close(bgl_instance bgl, int val) {
    if (!bgl->window) {
        fprintf(stderr, "Invalid window");
        return;
    }

    bgl->window->should_close = val;
}

BGL_API void bgl_show_window(bgl_instance bgl) {
    if (!bgl->window) {
        fprintf(stderr, "Invalid window\n");
        return;
    }

    show_platform_window(bgl);
}

BGL_API void bgl_set_window_title(bgl_instance bgl, const char *title) {
    if (!bgl->window) {
        fprintf(stderr, "Invalid window");
        return;
    }

    if (!title) {
        fprintf(stderr, "Invalid title");
        return;
    }

    set_platform_window_title(bgl, title);
}

BGL_API void bgl_swap_buffers(bgl_instance bgl) {
    if (bgl->dev.swap_buffers)
        bgl->dev.swap_buffers(bgl);
}


/// events

BGL_API void bgl_poll_events(bgl_instance bgl) {
    poll_platform_window_events(bgl);
}

BGL_API void bgl_wait_events(bgl_instance bgl) {
    wait_platform_window_events(bgl);
}

BGL_API void bgl_wait_events_timeout(bgl_instance bgl, double t) {
    if (t != t || t < 0.0 || t > DBL_MAX) {
        fprintf(stderr, "Invalid timeout: %f", t);
        return;
    }

    wait_platform_window_events_timeout(bgl, t);
}

BGL_API void bgl_send_empty_event(bgl_instance bgl) {
    send_platform_window_empty_event(bgl);
}


///////////////////////////////////////////////////////////////////////////////

void input_key(bgl_instance bgl, bgl_key key, unsigned scancode, bgl_key_action action, bgl_key_mods mods) {
    if (!bgl->window) {
        fprintf(stderr, "Invalid window");
        return;
    }

    if (key >= 0 && key < BGL_KEY_MAX) {
        if (action == BGL_RELEASE && bgl->window->keys[key].action == BGL_RELEASE)
            return;

        int repeated = (action == BGL_PRESS && bgl->window->keys[key].action == BGL_PRESS);

        bgl->window->keys[key].action = action;

        if (repeated)
            action = BGL_REPEATE;
    }

    if (bgl->window->callbacks.key)
        (*bgl->window->callbacks.key)(bgl, key, scancode, action, mods);
}

void input_window_close_request(bgl_instance bgl) {
    bgl->window->should_close = true;
    if (bgl->window->callbacks.close)
        (*bgl->window->callbacks.close)(bgl);
}
