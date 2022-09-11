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


static bgl_vertex_buffer get_vertex_buf(bgl_instance bgl, int vbuf_id) {
    bgl_vertex_buffer b = bgl->vertex_buffer;

    while (b) {
        if (b->id == vbuf_id)
            return b;
        b = b->next;
    }

    return b;
}

static void insert_index_buf(bgl_instance bgl, bgl_index_buffer ibuf) {
    ibuf->next = bgl->index_buffer;
    bgl->index_buffer = ibuf;
    ++bgl->ibuf_cnt;
}

static void remove_index_buf(bgl_instance bgl, int id, int with_vbuf) {
    bgl_index_buffer *b = &bgl->index_buffer;

    while (*b) {
        if ((*b)->id == id) {
            bgl_index_buffer next = (*b)->next;
            if (with_vbuf)
                bgl_remove_vertex_buffer(bgl, (*b)->vbuf->id);
            free((*b)->indices);
            free(*b);
            *b = next;
            --bgl->ibuf_cnt;
            return;
        }
        b = &(*b)->next;
    }
}

static void clear_index_buf(bgl_instance bgl) {
    bgl_index_buffer b = bgl->index_buffer;

    while (b) {
        bgl_index_buffer next = b->next;
        free(b->indices);
        free(b);
        b = next;
    }
    bgl->ibuf_cnt = 0;
}

static void draw_points(bgl_instance bgl, bgl_index_buffer buf, vertex_item *vertices) {
    ivec3 idxs;

    for (int i = buf->count; i--;) {
        idxs[0] = buf->indices[i].idx;

        vertices[idxs[0]].used = 1;

        push_back_helper_buf_idx(bgl, buf->vbuf->vitem_off, idxs, buf->indices[i].color, 1);
    }
}

static void draw_lines(bgl_instance bgl, bgl_index_buffer buf, vertex_item *vertices, bgl_drawing_modes mode) {
    ivec3 idxs;
    int inc = (mode == BGL_LINES) ? 2 : 1;

    for (int i = 0; i < buf->count - 1; i += inc) {
        idxs[0] = buf->indices[i].idx;
        idxs[1] = buf->indices[i + 1].idx;

        vertices[idxs[0]].used = vertices[idxs[1]].used = 1;

        push_back_helper_buf_idx(bgl, buf->vbuf->vitem_off, idxs, buf->indices[i].color, 2);
    }

    if (mode == BGL_LINES_LOOP) {
        idxs[0] = buf->indices[0].idx;

        push_back_helper_buf_idx(bgl, buf->vbuf->vitem_off, idxs, buf->indices[0].color, 2);
    }
}

static void draw_triangles(bgl_instance bgl, bgl_index_buffer buf, vertex_item *vertices, vec4 camera, vec4 light, bgl_drawing_modes mode) {
    vec3 ray;
    ivec3 idxs;

    vec4 color;
    vec4 norm;
    vec3 light_color = GLM_VEC3_ONE_INIT;
    vec3 diffuse;

    int is_strip = mode == BGL_TRIANGLES_STRIP, strip = 0, inc = is_strip ? 1 : 3;

    for (int i = 0; i < buf->count - 2; i += inc) {
        idxs[0] = buf->indices[i + (strip ? 1 : 0)].idx;
        idxs[1] = buf->indices[i + (strip ? 0 : 1)].idx;
        idxs[2] = buf->indices[i + 2].idx;
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
        glm_vec3_mul(diffuse, buf->indices[i].color, color);
//        glm_vec3_clamp(color, 0, 1);

        vertices[idxs[0]].used = vertices[idxs[1]].used = vertices[idxs[2]].used = 1;

        push_back_helper_buf_idx(bgl, buf->vbuf->vitem_off, idxs, color, 3);
    }
}

static void draw_triangles_fan(bgl_instance bgl, bgl_index_buffer buf, vertex_item *vertices, vec4 camera, vec4 light) {
    vec3 ray;
    ivec3 idxs;

    vec4 color;
    vec4 norm;
    vec3 light_color = GLM_VEC3_ONE_INIT;
    vec3 diffuse;

    idxs[0] = buf->indices[0].idx;
    vertices[idxs[0]].used = 1;

    for (int i = 1; i < buf->count - 1; ++i) {
        idxs[1] = buf->indices[i].idx;
        idxs[2] = buf->indices[i + 1].idx;

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
        glm_vec3_mul(diffuse, buf->indices[i].color, color);
//        glm_vec3_clamp(color, 0, 1);

        vertices[idxs[0]].used = vertices[idxs[1]].used = 1;

        push_back_helper_buf_idx(bgl, buf->vbuf->vitem_off, idxs, color, 3);
    }
}

///////////////////////////////////////////////////////////////////////////////

BGL_API int bgl_create_index_buffer(bgl_instance bgl, int vbuf_id, const vindex *indices, int count, bgl_drawing_modes mode) {
    static int new_id = 0;

    if (new_id == INT_MIN) {
        fprintf(stderr, "Failed to create index buffer: buffer limit reached\n");
        return -1;
    }

    bgl_vertex_buffer vb = get_vertex_buf(bgl, vbuf_id);
    if (!vb) {
        fprintf(stderr, "Failed to create index buffer: invalid vertex buffer ID: %i\n", vbuf_id);
        return -1;
    }

    bgl_index_buffer buf = malloc(sizeof(*buf));
    if (!(buf && (buf->indices = aligned_alloc(16, count * sizeof(*indices))))) {
        fprintf(stderr, "Failed to create index buffer: %s\n", strerror(errno));
        return -1;
    }

    memcpy(buf->indices, indices, count * sizeof(*indices));
    for (int i = 0; i < count; ++i)
        glm_vec3_normalize(buf->indices[i].normal);

    buf->vbuf = vb;
    buf->count = count;
    buf->id = new_id++;
    buf->render_mode = mode;

    insert_index_buf(bgl, buf);

    return buf->id;
}

BGL_API void bgl_remove_index_buffer(bgl_instance bgl, int ibuf_id, int with_vbuf) {
    remove_index_buf(bgl, ibuf_id, with_vbuf);
}

BGL_API void bgl_clear_index_buffers(bgl_instance bgl) {
    clear_index_buf(bgl);
}

BGL_API void bgl_draw_index_buffers(bgl_instance bgl, bgl_drawing_modes mode) {
    mat4 vp;
    vec4 camera, light;
    vertex_item *vhb_buf, *vertices;

    prepare_buffers(bgl, camera, light, vp, &vhb_buf);

    for (bgl_index_buffer buf = bgl->index_buffer; buf; buf = buf->next) {
        bgl_drawing_modes true_mode = mode ? : buf->render_mode;
        vertices = &vhb_buf[buf->vbuf->vitem_off];

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
