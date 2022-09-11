/*
 * Copyright (c) 2022 Alexander Baskikh
 *
 * MIT License (MIT), http://opensource.org/licenses/MIT
 * Full license can be found in the LICENSE file
 */

#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <bgl/bgl.h>

#include "internal.h"


BGL_API bgl_instance bgl_init() {
    bgl_instance bgl = calloc(1, sizeof(*bgl));
    if (!bgl) {
        fprintf(stderr, "Failed to init BGL: %s\n", strerror(errno));
        return NULL;
    }

    if (!init_platform(bgl)) {
        bgl_terminate(bgl);
        return NULL;
    }

    init_platform_timer(bgl);

    // set default configs
    bgl->default_cfgs.window.visible = true;
    bgl->default_cfgs.window.resizable = false;
    bgl->default_cfgs.framebuffer.red_bits = 8;
    bgl->default_cfgs.framebuffer.green_bits = 8;
    bgl->default_cfgs.framebuffer.blue_bits = 8;
    bgl->default_cfgs.framebuffer.alpha_bits = 8;
    bgl->default_cfgs.framebuffer.depth_bits = 24;
    bgl->default_cfgs.render.api = BGL_BASE_RENDER_API;

    bgl->dev.hb1 = HELP_BUF;
    bgl->dev.hb2 = HELP_BUF;
    bgl->dev.hb3 = HELP_BUF;

    return bgl;
}

BGL_API void bgl_terminate(bgl_instance bgl) {
    bgl_clear_index_buffers(bgl);
    bgl_clear_vertex_bufers(bgl);
    clear_helper_buf(bgl);
    bgl_destroy_window(bgl);
    terminate_platform(bgl);
    free(bgl);
}
