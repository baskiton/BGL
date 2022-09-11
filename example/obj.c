/*
 * Copyright (c) 2022 Alexander Baskikh
 *
 * MIT License (MIT), http://opensource.org/licenses/MIT
 * Full license can be found in the LICENSE file
 */

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <bgl/bgl.h>
#include <bgl/bglt.h>


int width = 800;
int height = 600;

uniform glob_uniform = {0};
mat4 obj_model = GLM_MAT4_IDENTITY_INIT;

static int load_obj(bgl_instance bgl, const char *obj_file_path) {
    bgl_obj_t *obj;

    if (!(obj = bgl_load_obj(obj_file_path)))
        return 1;

    if (!(obj->v_cnt && obj->vertices)) {
        fprintf(stderr, "vertexes invalid\n");
        errno = EINVAL;
        return 1;
    }

    printf("obj v=%d (%p) groups=%d:\n", obj->v_cnt, obj->vertices, obj->group_cnt);
    for (size_t i = obj->group_cnt; i--;)
        printf("  [%lu] \"%-*s\" i=%d (%p)\n", i, 8, obj->groups[i].name, obj->groups[i].i_cnt, obj->groups[i].indices);

    int vbuf = bgl_create_vertex_buffer(bgl, obj->vertices, obj->v_cnt, 0);
    bgl_bind_model_matrix(bgl, vbuf, &obj_model);
    for (size_t i = obj->group_cnt; i--;)
        bgl_create_index_buffer(bgl, vbuf, obj->groups[i].indices, obj->groups[i].i_cnt, BGL_TRIANGLES);

    return 0;
}

static int init(bgl_instance bgl, const char *obj_file_path) {
    if (load_obj(bgl, obj_file_path))
        return 1;

    mat4 cam;
    glm_rotate_make(cam, glm_rad(-20), (vec3){1, 0, 0});
    glm_translate(cam, (vec4){0, -150, 0});

    vec3 cam_pos = {cam[3][0], cam[3][1], cam[3][2]};
    vec3 cam_target = {0, 0, 0};
    vec3 cam_up = {0, 0, 1};

    bgl_set_viewport(bgl, &((bgl_viewport){0, (float)height, (float)width, (float)-height}));
    glm_lookat(cam_pos, cam_target, cam_up, glob_uniform.view);
    glm_perspective(glm_rad(67.5f), bgl_get_viewport_aspect_ratio(bgl), 0.1f, 250.0f, glob_uniform.proj);
    bgl_set_global_uniform(bgl, &glob_uniform, 0);

    return 0;
}

static void draw(bgl_instance bgl, float t) {
    static vec3 z_axis = {0, 0, 1};
    t = 20 / t;
    glm_rotate(obj_model, glm_rad(t), z_axis);

    bgl_draw_index_buffers(bgl, BGL_TRIANGLES);
    bgl_swap_buffers(bgl);
}

int main(int argc, char **argv) {
    if (argc != 2) {
        fputs("obj file path is not specified", stderr);
        exit(EXIT_FAILURE);
    }

    char title[64];
    char title_fmt[] = "BGL Obj Loader | FPS: %.01f";

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

    if (init(bgl, argv[1])) {
        fprintf(stderr, "init error: %s\n", strerror(errno));
        bgl_terminate(bgl);
        exit(EXIT_FAILURE);
    }

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
        draw(bgl, (float)fps);
    }

    bgl_destroy_window(bgl);
    bgl_terminate(bgl);

    return 0;
}
