/*
 * Copyright (c) 2022 Alexander Baskikh
 *
 * MIT License (MIT), http://opensource.org/licenses/MIT
 * Full license can be found in the LICENSE file
 */

#include "internal.h"
#include "render/base.h"


///////////////////////////////////////////////////////////////////////////////

int create_platform_window(bgl_instance bgl, const bgl_window_cfg *w_cfg, const bgl_fb_cfg *fb_cfg, const bgl_render_cfg *rndr_cfg) {
    return false;
}

void destroy_platform_window(bgl_instance bgl) {
}

void show_platform_window(bgl_instance bgl) {
}

void set_platform_window_title(bgl_instance bgl, const char *title) {
}

int is_visible_platform_window(bgl_instance bgl) {
    return false;
}


/// window events

void poll_platform_window_events(bgl_instance bgl) {
}

void wait_platform_window_events(bgl_instance bgl) {
}

void wait_platform_window_events_timeout(bgl_instance bgl, double t) {
}

void send_platform_window_empty_event(bgl_instance bgl) {
}
