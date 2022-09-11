/*
 * Copyright (c) 2022 Alexander Baskikh
 *
 * MIT License (MIT), http://opensource.org/licenses/MIT
 * Full license can be found in the LICENSE file
 */

#include "internal.h"


BGL_API void bgl_set_global_uniform(bgl_instance bgl, uniform *uniform, int mode) {
    // TODO: check mode (bitwise). view, proj, light, other...

    bgl->glob_uniform.model = &uniform->model;
    bgl->glob_uniform.view = &uniform->view;
    bgl->glob_uniform.proj = &uniform->proj;
    bgl->glob_uniform_mode = mode;
}

BGL_API int bgl_bind_model_matrix(bgl_instance bgl, int vbuf_id, mat4 *model) {
    // TODO: check mode (bitwise). view, proj, light, other...

    for (bgl_vertex_buffer b = bgl->vertex_buffer; b; b = b->next) {
        if (b->id == vbuf_id) {
            b->model_m = model;
            return true;
        }
    }
    fprintf(stderr, "bgl_bind_model_matrix: Invalid vertex buffer ID: %i\n", vbuf_id);

    return false;
}
