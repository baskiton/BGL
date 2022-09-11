/*
 * Copyright (c) 2022 Alexander Baskikh
 *
 * MIT License (MIT), http://opensource.org/licenses/MIT
 * Full license can be found in the LICENSE file
 */

#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "internal.h"


static void insert_vertex_buf(bgl_instance bgl, bgl_vertex_buffer vbuf) {
    vbuf->next = bgl->vertex_buffer;
    bgl->vertex_buffer = vbuf;
    ++bgl->vbuf_cnt;
}

static void remove_vertex_buf(bgl_instance bgl, int id) {
    bgl_vertex_buffer *b = &bgl->vertex_buffer;

    while (*b) {
        if ((*b)->id == id) {
            bgl_vertex_buffer next = (*b)->next;
            free((*b)->vertices);
            free(*b);
            *b = next;
            --bgl->vbuf_cnt;
            return;
        }
        b = &(*b)->next;
    }
}

static void clear_vertex_buf(bgl_instance bgl) {
    bgl_vertex_buffer b = bgl->vertex_buffer;

    while (b) {
        bgl_vertex_buffer next = b->next;
        free(b->vertices);
        free(b);
        b = next;
    }
    bgl->vbuf_cnt = 0;
}

static void draw_points(bgl_instance bgl, bgl_vertex_buffer buf, vertex_item *vertices) {
    ivec3 idxs;

    for (idxs[0] = buf->count; idxs[0]--;) {
        vertices[idxs[0]].used = 1;

        push_back_helper_buf_idx(bgl, buf->vitem_off, idxs, buf->vertices[idxs[0]].color, 1);
    }
}

static void draw_lines(bgl_instance bgl, bgl_vertex_buffer buf, vertex_item *vertices, bgl_drawing_modes mode) {
    ivec3 idxs;
    int inc = (mode == BGL_LINES) ? 2 : 1;

    for (int i = 0; i < buf->count - 1; i += inc) {
        idxs[0] = i;
        idxs[1] = i + 1;

        vertices[idxs[0]].used = vertices[idxs[1]].used = 1;

        push_back_helper_buf_idx(bgl, buf->vitem_off, idxs, buf->vertices[i].color, 2);
    }

    if (mode == BGL_LINES_LOOP) {
        idxs[0] = 0;

        push_back_helper_buf_idx(bgl, buf->vitem_off, idxs, buf->vertices[0].color, 2);
    }
}

static void draw_triangles(bgl_instance bgl, bgl_vertex_buffer buf, vertex_item *vertices, vec4 camera, vec4 light, bgl_drawing_modes mode) {
    vec3 ray;
    ivec3 idxs;

    vec4 color;
    vec4 norm;
    vec3 light_color = GLM_VEC3_ONE_INIT;
    vec3 diffuse;

    int is_strip = mode == BGL_TRIANGLES_STRIP, strip = 0, inc = is_strip ? 1 : 3;

    for (int i = 0; i < buf->count - 2; i += inc) {
        idxs[0] = i + (strip ? 1 : 0);
        idxs[1] = i + (strip ? 0 : 1);
        idxs[2] = i + 2;
        strip ^= is_strip;

        triangle_normal(vertices[idxs[0]].vtx,
                        vertices[idxs[1]].vtx,
                        vertices[idxs[2]].vtx,
                        norm);

        // back-face culling
        glm_vec3_sub(vertices[idxs[0]].vtx, camera, ray);
        if (glm_vec3_dot(norm, ray) >= 0)
            continue;

        float light_intencity = glm_max(glm_vec3_dot(norm, light), 0);
        glm_vec3_scale(light_color, light_intencity, diffuse);
        glm_vec3_mul(diffuse, buf->vertices[i].color, color);
//        glm_vec3_clamp(color, 0, 1);

        vertices[idxs[0]].used = vertices[idxs[1]].used = vertices[idxs[2]].used = 1;

        push_back_helper_buf_idx(bgl, buf->vitem_off, idxs, color, 3);
    }
}

static void draw_triangles_fan(bgl_instance bgl, bgl_vertex_buffer buf, vertex_item *vertices, vec4 camera, vec4 light) {
    vec3 ray;
    ivec3 idxs;

    vec4 color;
    vec4 norm;
    vec3 light_color = GLM_VEC3_ONE_INIT;
    vec3 diffuse;

    idxs[0] = 0;
    vertices[0].used = 1;

    for (int i = 1; i < buf->count - 1; ++i) {
        idxs[1] = i;
        idxs[2] = i + 1;

        triangle_normal(vertices[idxs[0]].vtx,
                        vertices[idxs[1]].vtx,
                        vertices[idxs[2]].vtx,
                        norm);

        // back-face culling
        glm_vec3_sub(vertices[idxs[0]].vtx, camera, ray);
        if (glm_vec3_dot(norm, ray) >= 0)
            continue;

        float light_intencity = glm_max(glm_vec3_dot(norm, light), 0);
        glm_vec3_scale(light_color, light_intencity, diffuse);
        glm_vec3_mul(diffuse, buf->vertices[i].color, color);
//        glm_vec3_clamp(color, 0, 1);

        vertices[idxs[0]].used = vertices[idxs[1]].used = 1;

        push_back_helper_buf_idx(bgl, buf->vitem_off, idxs, color, 3);
    }
}

///////////////////////////////////////////////////////////////////////////////

BGL_API int bgl_create_vertex_buffer(bgl_instance bgl, const vertex *vertices, int count, bgl_drawing_modes mode) {
    static int new_id = 0;

    if (new_id == INT_MIN) {
        fprintf(stderr, "Failed to create vertex buffer: buffer limit reached\n");
        return -1;
    }

    bgl_vertex_buffer buf = malloc(sizeof(*buf));
    if (!(buf && (buf->vertices = aligned_alloc(16, count * sizeof(*vertices))))) {
        fprintf(stderr, "Failed to create vertex buffer: %s\n", strerror(errno));
        return -1;
    }

    memcpy(buf->vertices, vertices, count * sizeof(*vertices));
    for (size_t i = 0; i < count; ++i)
        buf->vertices[i].pos[3] = 1.0f;

    buf->id = new_id++;
    buf->count = count;
    buf->render_mode = mode;
    buf->model_m = NULL;

    insert_vertex_buf(bgl, buf);

    return buf->id;
}

BGL_API void bgl_remove_vertex_buffer(bgl_instance bgl, int id) {
    remove_vertex_buf(bgl, id);
}

BGL_API void bgl_clear_vertex_bufers(bgl_instance bgl) {
    clear_vertex_buf(bgl);
}

BGL_API void bgl_draw_vertex_buffers(bgl_instance bgl, bgl_drawing_modes mode) {
    mat4 vp;
    vec4 camera, light;
    vertex_item *vhb_buf, *vertices;

    prepare_buffers(bgl, camera, light, vp, &vhb_buf);

    for (bgl_vertex_buffer buf = bgl->vertex_buffer; buf; buf = buf->next) {
        bgl_drawing_modes true_mode = mode ?: buf->render_mode;
        vertices = &vhb_buf[buf->vitem_off];

        switch (true_mode) {
        case BGL_POINTS:
            draw_points(bgl, buf, vertices);
            break;
        case BGL_LINES:
        case BGL_LINES_STRIP:
        case BGL_LINES_LOOP:
            draw_lines(bgl, buf, vertices, true_mode);
            break;
        case BGL_TRIANGLES:
        case BGL_TRIANGLES_STRIP:
            draw_triangles(bgl, buf, vertices, camera, light, true_mode);
            break;
        case BGL_TRIANGLES_FAN:
            draw_triangles_fan(bgl, buf, vertices, camera, light);
            break;
        default:
            fprintf(stderr, "Invalid drawing mode: 0x%04X\n", true_mode);
            break;
        }
    }

    draw_buffers(bgl, vp);
}
