/*
 * Copyright (c) 2022 Alexander Baskikh
 *
 * MIT License (MIT), http://opensource.org/licenses/MIT
 * Full license can be found in the LICENSE file
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <bgl/bgl.h>
#include <string.h>


#define S2NS (double)1e9

bgl_instance bgl;

uniform glob_uniform = {0};

mat4 red_model = GLM_MAT4_IDENTITY_INIT;
mat4 green_model = GLM_MAT4_IDENTITY_INIT;
mat4 blue_model = GLM_MAT4_IDENTITY_INIT;

static void vertex_set(vec4 pos, vertex *src, vertex *dst) {
    glm_vec4_copy(pos, src->pos);
    memcpy(dst, src, sizeof(*src));
}

static void gear(float inner_radius, float outer_radius, float width, int teeth, float tooth_depth, mat4 *model, vec4 color) {
    int i, j;
    int vbuf;
    float r0, r1, r2;
    float angle, da;
    float u, v, len;
    float w_half = width / 2.0f;

    r0 = inner_radius;
    r1 = outer_radius - tooth_depth / 2.0f;
    r2 = outer_radius + tooth_depth / 2.0f;

    da = 2.0f * (float)M_PI / (float)teeth / 4.0f;

    // draw front face
    vertex vtmp, *vertices = aligned_alloc(16, (teeth * 4 + 2) * sizeof(*vertices));
    glm_vec4_copy(color, vtmp.color);
    glm_vec3_copy((vec3){0, 0, 1}, vtmp.normal);

    for (i = j = 0; i < teeth; ++i) {
        angle = (float)i * 2.0f * (float)M_PI / (float)teeth;

        vertex_set((vec4){r0 * cosf(angle), r0 * sinf(angle), w_half}, &vtmp, &vertices[j++]);
        vertex_set((vec4){r1 * cosf(angle), r1 * sinf(angle), w_half}, &vtmp, &vertices[j++]);
        vertex_set((vec4){r0 * cosf(angle), r0 * sinf(angle), w_half}, &vtmp, &vertices[j++]);
        vertex_set((vec4){r1 * cosf(angle + 3 * da), r1 * sinf(angle + 3 * da), w_half}, &vtmp, &vertices[j++]);
    }
    vertex_set((vec4){r0 * cosf(0), r0 * sinf(0), w_half}, &vtmp, &vertices[j++]);
    vertex_set((vec4){r1 * cosf(0), r1 * sinf(0), w_half}, &vtmp, &vertices[j++]);

    vbuf = bgl_create_vertex_buffer(bgl, vertices, j, BGL_TRIANGLES_STRIP);
    bgl_bind_model_matrix(bgl, vbuf, model);

    // draw front sides of teeth
    vertices = reallocarray(vertices, (teeth * 6), sizeof(*vertices));
    for (i = j = 0; i < teeth; ++i) {
        angle = (float)i * 2.0f * (float)M_PI / (float)teeth;

        vertex_set((vec4){r1 * cosf(angle), r1 * sinf(angle), w_half}, &vtmp, &vertices[j++]);
        vertex_set((vec4){r2 * cosf(angle + da), r2 * sinf(angle + da), w_half}, &vtmp, &vertices[j++]);
        vertex_set((vec4){r2 * cosf(angle + 2 * da), r2 * sinf(angle + 2 * da), w_half}, &vtmp, &vertices[j++]);

        vertex_set((vec4){r1 * cosf(angle), r1 * sinf(angle), w_half}, &vtmp, &vertices[j++]);
        vertex_set((vec4){r2 * cosf(angle + 2 * da), r2 * sinf(angle + 2 * da), w_half}, &vtmp, &vertices[j++]);
        vertex_set((vec4){r1 * cosf(angle + 3 * da), r1 * sinf(angle + 3 * da), w_half}, &vtmp, &vertices[j++]);
    }
    vbuf = bgl_create_vertex_buffer(bgl, vertices, j, BGL_TRIANGLES);
    bgl_bind_model_matrix(bgl, vbuf, model);

    vtmp.normal[2] = -1.0f;

    // draw back face
    vertices = reallocarray(vertices, (teeth * 4 + 2), sizeof(*vertices));
    for (i = j = 0; i < teeth; ++i) {
        angle = (float)i * 2.0f * (float)M_PI / (float)teeth;

        vertex_set((vec4){r1 * cosf(angle), r1 * sinf(angle), -w_half}, &vtmp, &vertices[j++]);
        vertex_set((vec4){r0 * cosf(angle), r0 * sinf(angle), -w_half}, &vtmp, &vertices[j++]);
        vertex_set((vec4){r1 * cosf(angle + 3 * da), r1 * sinf(angle + 3 * da), -w_half}, &vtmp, &vertices[j++]);
        vertex_set((vec4){r0 * cosf(angle), r0 * sinf(angle), -w_half}, &vtmp, &vertices[j++]);
    }
    vertex_set((vec4){r1 * cosf(0), r1 * sinf(0), -w_half}, &vtmp, &vertices[j++]);
    vertex_set((vec4){r0 * cosf(0), r0 * sinf(0), -w_half}, &vtmp, &vertices[j++]);

    vbuf = bgl_create_vertex_buffer(bgl, vertices, j, BGL_TRIANGLES_STRIP);
    bgl_bind_model_matrix(bgl, vbuf, model);

    // draw back sides of teeth
    vertices = reallocarray(vertices, (teeth * 6), sizeof(*vertices));
    for (i = j = 0; i < teeth; ++i) {
        angle = (float)i * 2.0f * (float)M_PI / (float)teeth;

       vertex_set((vec4){r1 * cosf(angle + 3 * da), r1 * sinf(angle + 3 * da), -w_half}, &vtmp, &vertices[j++]);
       vertex_set((vec4){r2 * cosf(angle + 2 * da), r2 * sinf(angle + 2 * da), -w_half}, &vtmp, &vertices[j++]);
       vertex_set((vec4){r2 * cosf(angle + da), r2 * sinf(angle + da), -w_half}, &vtmp, &vertices[j++]);

       vertex_set((vec4){r1 * cosf(angle + 3 * da), r1 * sinf(angle + 3 * da), -w_half}, &vtmp, &vertices[j++]);
       vertex_set((vec4){r2 * cosf(angle + da), r2 * sinf(angle + da), -w_half}, &vtmp, &vertices[j++]);
       vertex_set((vec4){r1 * cosf(angle), r1 * sinf(angle), -w_half}, &vtmp, &vertices[j++]);
    }
    vbuf = bgl_create_vertex_buffer(bgl, vertices, j, BGL_TRIANGLES);
    bgl_bind_model_matrix(bgl, vbuf, model);

    // draw outward faces of teeth
    vertices = reallocarray(vertices, (teeth * 8 + 2), sizeof(*vertices));
    for (i = j = 0; i < teeth; ++i) {
        angle = (float)i * 2.0f * (float)M_PI / (float)teeth;

        vertex_set((vec4){r1 * cosf(angle), r1 * sinf(angle), w_half}, &vtmp, &vertices[j++]);
        vertex_set((vec4){r1 * cosf(angle), r1 * sinf(angle), -w_half}, &vtmp, &vertices[j++]);

        u = r2 * cosf(angle + da) - r1 * cosf(angle);
        v = r2 * sinf(angle + da) - r1 * sinf(angle);
        len = sqrtf(u * u + v * v);
        u /= len;
        v /= len;
        glm_vec3_copy((vec3){v, -u, 0.0f}, vtmp.normal);

        vertex_set((vec4){r2 * cosf(angle + da), r2 * sinf(angle + da), w_half}, &vtmp, &vertices[j++]);
        vertex_set((vec4){r2 * cosf(angle + da), r2 * sinf(angle + da), -w_half}, &vtmp, &vertices[j++]);

        glm_vec3_copy((vec3){cosf(angle), sinf(angle), 0.0f}, vtmp.normal);

        vertex_set((vec4){r2 * cosf(angle + 2 * da), r2 * sinf(angle + 2 * da), w_half}, &vtmp, &vertices[j++]);
        vertex_set((vec4){r2 * cosf(angle + 2 * da), r2 * sinf(angle + 2 * da), -w_half}, &vtmp, &vertices[j++]);

        u = r1 * cosf(angle + 3 * da) - r2 * cosf(angle + 2 * da);
        v = r1 * sinf(angle + 3 * da) - r2 * sinf(angle + 2 * da);
        glm_vec3_copy((vec3){v, -u, 0.0f}, vtmp.normal);

        vertex_set((vec4){r1 * cosf(angle + 3 * da), r1 * sinf(angle + 3 * da), w_half}, &vtmp, &vertices[j++]);
        vertex_set((vec4){r1 * cosf(angle + 3 * da), r1 * sinf(angle + 3 * da), -w_half}, &vtmp, &vertices[j++]);

        glm_vec3_copy((vec3){cosf(angle), sinf(angle), 0.0f}, vtmp.normal);
    }
    vertex_set((vec4){r1 * cosf(0), r1 * sinf(0), w_half}, &vtmp, &vertices[j++]);
    vertex_set((vec4){r1 * cosf(0), r1 * sinf(0), -w_half}, &vtmp, &vertices[j++]);

    vbuf = bgl_create_vertex_buffer(bgl, vertices, j, BGL_TRIANGLES_STRIP);
    bgl_bind_model_matrix(bgl, vbuf, model);

    // draw inside radius cylinder
    vertices = reallocarray(vertices, (teeth * 2 + 2), sizeof(*vertices));
    for (i = j = 0; i < teeth; ++i) {
        angle = (float)i * 2.0f * (float)M_PI / (float)teeth;

        vertex_set((vec4){r0 * cosf(angle), r0 * sinf(angle), -w_half}, &vtmp, &vertices[j++]);
        vertex_set((vec4){r0 * cosf(angle), r0 * sinf(angle), w_half}, &vtmp, &vertices[j++]);
    }
    vertex_set((vec4){r0 * cosf(0), r0 * sinf(0), -w_half}, &vtmp, &vertices[j++]);
    vertex_set((vec4){r0 * cosf(0), r0 * sinf(0), w_half}, &vtmp, &vertices[j++]);

    vbuf = bgl_create_vertex_buffer(bgl, vertices, j, BGL_TRIANGLES_STRIP);
    bgl_bind_model_matrix(bgl, vbuf, model);

    free(vertices);
}

static void init(int width, int height) {
    mat4 cam;
    glm_rotate_make(cam, glm_rad(-20), (vec3){1, 0, 0});
    glm_rotate(cam, glm_rad(-30), (vec3){0, 1, 0});
    glm_translate(cam, (vec4){0, 0, 20});

    vec3 cam_pos = {cam[3][0], cam[3][1], cam[3][2]};
    vec3 cam_target = {0, 0, 0};
    vec3 cam_up = {0.0f, 1.0f, 0.0f};

    bgl_set_viewport(bgl, &((bgl_viewport){0, (float)height, (float)width, (float)-height}));
    glm_lookat(cam_pos, cam_target, cam_up, glob_uniform.view);
    glm_perspective(glm_rad(67.0f), (float)width / (float)height, 0.1f, 100.0f, glob_uniform.proj);
    bgl_set_global_uniform(bgl, &glob_uniform, 0);

    gear(1.0f, 4.0f, 1.0f, 20, 0.7f, &red_model, (vec4s){.r = 1.0f, .a = 1.0f}.raw);
    gear(0.5f, 2.0f, 2.0f, 10, 0.7f, &green_model, (vec4s){.g = 1.0f, .a = 1.0f}.raw);
    gear(1.3f, 2.0f, 0.5f, 10, 0.7f, &blue_model, (vec4s){.b = 1.0f, .a = 1.0f}.raw);

    vec3 red = {-3.0f, -2.0f, 0.0f};
    vec3 green = {3.1f, -2.0f, 0.f};
    vec3 blue = {-3.1f, 4.2f, 0.0f};

    glm_translate_make(red_model, red);
    glm_translate_make(green_model, green);
    glm_translate_make(blue_model, blue);
    glm_rotate(green_model, glm_rad(-9.0f), (vec3){0, 0, 1});
    glm_rotate(blue_model, glm_rad(-25.0f), (vec3){0, 0, 1});

}

static void animate(float t) {
    static vec3 z_axis = {0, 0, 1};
    t = 50 / t;

    glm_rotate(red_model, glm_rad(t), z_axis);
    glm_rotate(green_model, glm_rad(-2.0f * t), z_axis);
    glm_rotate(blue_model, glm_rad(-2.0f * t), z_axis);
}

int main() {
    int width = 800;
    int height = 800;
    int fps_lim = 60;
    char title[64];
    const char title_fmt[] = "BGL Gears | FPS: %.01f";

    if (!(bgl = bgl_init()))
        exit(EXIT_FAILURE);

    if (!bgl_create_window(bgl, width, height, NULL)) {
        fprintf(stderr, "no window\n");
        bgl_terminate(bgl);
        exit(EXIT_FAILURE);
    }

    init(width, height);

    int64_t delay = (int64_t)bgl_get_timer_freq(bgl) / fps_lim;
    uint64_t t = 0, tt;
    int64_t d;
    double fps;

    while (!bgl_window_should_close(bgl)) {
        tt = bgl_get_timer(bgl);
        d = (int64_t)(tt - t);
        if (d < delay) {
            struct timespec ts = {.tv_nsec = delay - d};
            nanosleep(&ts, NULL);
            continue;
        }
        t = tt;
        fps = (double)bgl_get_timer_freq(bgl) / (double)d;
        snprintf(title, sizeof(title), title_fmt, fps);
        bgl_set_window_title(bgl, title);

        bgl_poll_events(bgl);
        animate((float)fps);

        bgl_draw_vertex_buffers(bgl, 0);
        bgl_swap_buffers(bgl);
    }

    bgl_terminate(bgl);

    return 0;
}
