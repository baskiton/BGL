/*
 * Copyright (c) 2022 Alexander Baskikh
 *
 * MIT License (MIT), http://opensource.org/licenses/MIT
 * Full license can be found in the LICENSE file
 */

#ifndef BGL_INTERNAL_H
#define BGL_INTERNAL_H

#include <stdint.h>

#include <bgl/bgl.h>
#include <bgl/bglm.h>


#if defined(__GNUC__)
# define likely(x) __builtin_expect(!!(x), 1)
# define unlikely(x) __builtin_expect(!!(x), 0)
#else
# define likely(x) (x)
# define unlikely(x) (x)
#endif


BGL_DEFINE_HANDLE(bgl_window);
BGL_DEFINE_STRUCT(bgl_platform);
BGL_DEFINE_STRUCT(bgl_platform_window);
BGL_DEFINE_STRUCT(bgl_window_cfg);
BGL_DEFINE_STRUCT(bgl_fb_cfg);
BGL_DEFINE_STRUCT(bgl_render_cfg);
BGL_DEFINE_HANDLE(bgl_vertex_buffer);
BGL_DEFINE_HANDLE(bgl_index_buffer);
BGL_DEFINE_STRUCT(bgl_viewport_internal);


#include "platform.h"


struct bgl_window_cfg {
    int width;
    int height;
    const char *title;

    int visible;
    int resizable;
};

struct bgl_fb_cfg {
    int red_bits;
    int green_bits;
    int blue_bits;
    int alpha_bits;
    int depth_bits;
};

struct bgl_render_cfg {
    int api;
};

struct bgl_window {
    // states
    int should_close;
    int resizable;

    struct {
        bgl_key_action action;
        uint64_t time;
    } keys[BGL_KEY_MAX];

    // TODO: sets to callback
    struct {
        bgl_close_window_fn close;
        bgl_key_fn key;
    } callbacks;

    bgl_platform_window platform;
};

struct bgl_viewport_internal {
    float x;
    float y;
    float px;
    float py;
    float pz;
    float pxh;
    float pyh;
    float ox;
    float oy;
    float oz;
    float aspect_ratio;
};

typedef struct {
    vec3 point;
    vec3 norm;
    float d;
} clip_plane;

typedef struct {
    vec4 vtx;
    ivec3 ivtx;
    int used;
} vertex_item;

struct bgl_vertex_buffer {
    bgl_vertex_buffer next;
    vertex *vertices;
    int vitem_off;
    int count;
    int id;
    int render_mode;
    mat4 *model_m;
};

struct bgl_index_buffer {
    bgl_index_buffer next;
    bgl_vertex_buffer vbuf;
    vindex *indices;
    int count;
    int id;
    int render_mode;
};

typedef struct {
    mat3 tri;
    vec4 color;
    float avg_z;
    int n;
} vtx_item;

typedef struct {
    ivec3 tri;
    vec4 color;
    float avg_z;
    int v_off;
    int n;
} idx_item;

typedef struct {
    void *buf;
    int buf_sz;
    int cnt;
} helper_buf;

#define HELP_BUF_INIT { .buf_sz = 512 }
#define HELP_BUF (helper_buf)HELP_BUF_INIT


struct bgl_instance {
    struct {
        uint64_t offset;
        uint64_t freq;
        BGL_LIB_PLATFORM_TIMER
    } timer;

    struct {
        bgl_window_cfg window;
        bgl_fb_cfg framebuffer;
        bgl_render_cfg render;
    } default_cfgs;

    bgl_platform platform;
    bgl_window window;

    struct device {
        helper_buf hb1;
        helper_buf hb2;
        helper_buf hb3;

        void (*destroy_render)(bgl_instance);
        void (*swap_buffers)(bgl_instance);

        void (*draw_pixel)(bgl_instance, const ivec3 v, const vec4 color);
        void (*draw_line)(bgl_instance, const ivec3 a, const ivec3 b, const vec4 color);
        void (*draw_triangle)(bgl_instance, const ivec3 a, const ivec3 b, const ivec3 c, const vec4 color);
        void (*draw_fill_triangle)(bgl_instance, const ivec3 a, const ivec3 b, const ivec3 c, const vec4 color);
    } dev;

    bgl_vertex_buffer vertex_buffer;
    int vbuf_cnt;
    bgl_index_buffer index_buffer;
    int ibuf_cnt;

    bgl_viewport_internal viewport;

    uniform_p glob_uniform;
    int glob_uniform_mode;
};

///////////////////////////////////////////////////////////////////////////////

void input_key(bgl_instance bgl, bgl_key key, unsigned scancode, bgl_key_action action, bgl_key_mods mods);

void loc_to_dev(mat4 vp, vec4 src, vec3 dst);
void dev_to_fb(bgl_viewport_internal *viewport, vec3 src, vec3 dst);
void loc_to_fb(mat4 vp, bgl_viewport_internal *viewport, vec4 src, vec3 dst);
void triangle_normal(vec3 a, vec3 b, vec3 c, vec3 dst);

idx_item *push_back_helper_buf_idx(bgl_instance bgl, int v_off, ivec3 idxs, vec4 color, int n);
void clear_helper_buf(bgl_instance bgl);

void prepare_buffers(bgl_instance bgl, vec4 camera, vec4 light, mat4 vp, vertex_item **vhb_buf);
void draw_buffers(bgl_instance bgl, mat4 vp);


#endif // BGL_INTERNAL_H
