/*
 * Copyright (c) 2022 Alexander Baskikh
 *
 * MIT License (MIT), http://opensource.org/licenses/MIT
 * Full license can be found in the LICENSE file
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <bgl/bgl.h>


int width = 640;
int height = 480;

uniform glob_uniform = {0};
mat4 cube_model;


void triangle_normal(vec3 a, vec3 b, vec3 c, vec3 dst) {
    vec3 u, v;
    glm_vec3_sub(b, a, u);
    glm_vec3_sub(c, a, v);
    glm_vec3_crossn(u, v, dst);
}

void init(bgl_instance bgl) {
    vertex cube_vertices[] = {
            {{1.000000f, 1.000000f, -1.000000f}},
            {{1.000000f, -1.000000f, -1.000000f}},
            {{1.000000f, 1.000000f, 1.000000f}},
            {{1.000000f, -1.000000f, 1.000000f}},
            {{-1.000000f, 1.000000f, -1.000000f}},
            {{-1.000000f, -1.000000f, -1.000000f}},
            {{-1.000000f, 1.000000f, 1.000000f}},
            {{-1.000000f, -1.000000f, 1.000000f}},
    };

    vindex cube_indices[] = {
            {4, GLM_VEC4_ONE_INIT}, {2, GLM_VEC4_ONE_INIT}, {0, GLM_VEC4_ONE_INIT},
            {2, GLM_VEC4_ONE_INIT}, {7, GLM_VEC4_ONE_INIT}, {3, GLM_VEC4_ONE_INIT},
            {6, GLM_VEC4_ONE_INIT}, {5, GLM_VEC4_ONE_INIT}, {7, GLM_VEC4_ONE_INIT},
            {1, GLM_VEC4_ONE_INIT}, {7, GLM_VEC4_ONE_INIT}, {5, GLM_VEC4_ONE_INIT},
            {0, GLM_VEC4_ONE_INIT}, {3, GLM_VEC4_ONE_INIT}, {1, GLM_VEC4_ONE_INIT},
            {4, GLM_VEC4_ONE_INIT}, {1, GLM_VEC4_ONE_INIT}, {5, GLM_VEC4_ONE_INIT},
            {4, GLM_VEC4_ONE_INIT}, {6, GLM_VEC4_ONE_INIT}, {2, GLM_VEC4_ONE_INIT},
            {2, GLM_VEC4_ONE_INIT}, {6, GLM_VEC4_ONE_INIT}, {7, GLM_VEC4_ONE_INIT},
            {6, GLM_VEC4_ONE_INIT}, {4, GLM_VEC4_ONE_INIT}, {5, GLM_VEC4_ONE_INIT},
            {1, GLM_VEC4_ONE_INIT}, {3, GLM_VEC4_ONE_INIT}, {7, GLM_VEC4_ONE_INIT},
            {0, GLM_VEC4_ONE_INIT}, {2, GLM_VEC4_ONE_INIT}, {3, GLM_VEC4_ONE_INIT},
            {4, GLM_VEC4_ONE_INIT}, {0, GLM_VEC4_ONE_INIT}, {1, GLM_VEC4_ONE_INIT},
    };

    for (int i = 0; i < 36; i += 3) {
        vec3 norm;
        GLMS_MAT4_IDENTITY;
        triangle_normal(cube_vertices[cube_indices[i].idx].pos, cube_vertices[cube_indices[i + 1].idx].pos, cube_vertices[cube_indices[i + 2].idx].pos, norm);
        for (int k = 0; k < 3; ++k)
            glm_vec3_copy(norm, cube_indices[i + k].normal);
    }

    mat4 cam;
    glm_rotate_make(cam, glm_rad(-20), (vec3){1, 0, 0});
    glm_rotate(cam, glm_rad(-30), (vec3){0, 1, 0});
    glm_translate(cam, (vec4){0, 0, 5});

    vec3 cam_pos = {cam[3][0], cam[3][1], cam[3][2]};
    vec3 cam_target = {0, 0, 0};
    vec3 cam_up = {0, 1, 0};

    bgl_set_viewport(bgl, &((bgl_viewport){0, (float)height, (float)width, (float)-height}));
    glm_lookat(cam_pos, cam_target, cam_up, glob_uniform.view);
    glm_perspective(glm_rad(67.5f), bgl_get_viewport_aspect_ratio(bgl), 0.1f, 100.0f, glob_uniform.proj);
    bgl_set_global_uniform(bgl, &glob_uniform, 0);

    int vbuf = bgl_create_vertex_buffer(bgl, cube_vertices, sizeof(cube_vertices) / sizeof(*cube_vertices), BGL_LINES_STRIP);
    bgl_create_index_buffer(bgl, vbuf, cube_indices, sizeof(cube_indices) / sizeof(*cube_indices), BGL_LINES_STRIP);
    bgl_bind_model_matrix(bgl, vbuf, &cube_model);
}

void draw(bgl_instance bgl, float t) {
    glm_mat4_identity(cube_model);
    glm_rotate(cube_model, (t * glm_rad(15.0f)), (vec3){1, 0, 0});
    glm_rotate(cube_model, (t * glm_rad(20.0f)), (vec3){0, 1, 0});
    glm_rotate(cube_model, (t * glm_rad(25.0f)), (vec3){0, 0, 1});

    bgl_draw_index_buffers(bgl, BGL_TRIANGLES);
    bgl_swap_buffers(bgl);
}

int main() {
    char title[64];
    char title_fmt[] = "BGL cube | FPS: %.01f";

    bgl_instance bgl = bgl_init();
    if (!bgl)
        exit(EXIT_FAILURE);

    if (!bgl_create_window(bgl, width, height, NULL)) {
        fprintf(stderr, "no window\n");
        bgl_terminate(bgl);
        exit(EXIT_FAILURE);
    }

    int fps_lim = 60;
    int64_t delay = (int64_t)bgl_get_timer_freq(bgl) / fps_lim;

    init(bgl);

    uint64_t t = 0, tt, start_timer = bgl_get_timer(bgl);
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
        draw(bgl, (float)((double)(t - start_timer) / (double)bgl_get_timer_freq(bgl)));
    }

    bgl_destroy_window(bgl);
    bgl_terminate(bgl);

    return 0;
}
