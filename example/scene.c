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
int fps_lim = 60;

uniform glob_uniform = BGL_UNIFORM_INIT;

vec3 cam_pos, cam_dir, cam_rot, cam_up = {0, 1, 0};

float cam_mov_x = 0, cam_mov_y = 0, cam_mov_z = 0, cam_rot_x = 0, cam_rot_y = 0, cam_rot_z = 0;

static void key_callback(bgl_instance bgl, bgl_key key, unsigned scancode, bgl_key_action action, bgl_key_mods mods) {
    if (action == BGL_PRESS)
        switch (key) {
        case BGL_KEY_ESCAPE:
            bgl_set_window_should_close(bgl, true);
            break;

        case BGL_KEY_A:
            --cam_mov_x;
            break;

        case BGL_KEY_D:
            ++cam_mov_x;
            break;

        case BGL_KEY_W:
            ++cam_mov_z;
            break;

        case BGL_KEY_S:
            --cam_mov_z;
            break;

        case BGL_KEY_LEFT:
            --cam_rot_y;
            break;

        case BGL_KEY_RIGHT:
            ++cam_rot_y;
            break;

        case BGL_KEY_UP:
            --cam_rot_x;
//            ++cam_mov_y;
            break;

        case BGL_KEY_DOWN:
            ++cam_rot_x;
//            --cam_mov_y;
            break;

//        case BGL_KEY_Q:
//            ++cam_rot_z;
//            break;

//        case BGL_KEY_E:
//            --cam_rot_z;
//            break;

        case BGL_KEY_LEFT_SHIFT:
            ++cam_mov_y;
            break;

        case BGL_KEY_LEFT_CONTROL:
            --cam_mov_y;
            break;

        default:
//            printf("PRESS %d\n", key);
            break;
        }

    else if (action == BGL_RELEASE)
        switch (key) {
        case BGL_KEY_A:
            ++cam_mov_x;
            break;

        case BGL_KEY_D:
            --cam_mov_x;
            break;

        case BGL_KEY_W:
            --cam_mov_z;
            break;

        case BGL_KEY_S:
            ++cam_mov_z;
            break;

        case BGL_KEY_LEFT:
            ++cam_rot_y;
            break;

        case BGL_KEY_RIGHT:
            --cam_rot_y;
            break;

        case BGL_KEY_UP:
            ++cam_rot_x;
//            --cam_mov_y;
            break;

        case BGL_KEY_DOWN:
            --cam_rot_x;
//            ++cam_mov_y;
            break;

//        case BGL_KEY_Q:
//            --cam_rot_z;
//            break;

//        case BGL_KEY_E:
//            ++cam_rot_z;
//            break;

        case BGL_KEY_LEFT_SHIFT:
            --cam_mov_y;
            break;

        case BGL_KEY_LEFT_CONTROL:
            ++cam_mov_y;
            break;

        default:
//            printf("RELEASE %d\n", key);
            break;
        }
}

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
    for (size_t i = obj->group_cnt; i--;)
        bgl_create_index_buffer(bgl, vbuf, obj->groups[i].indices, obj->groups[i].i_cnt, 0);

    return 0;
}

static int init(bgl_instance bgl, const char *obj_file_path) {
    bgl_set_key_callback(bgl, key_callback);

    if (load_obj(bgl, obj_file_path))
        return 1;

    glm_vec3_copy((vec3){0, 0, -10}, cam_pos);
    glm_vec3_copy((vec3){0, 0, 1}, cam_dir);

    bgl_set_viewport(bgl, &((bgl_viewport){0, (float)height, (float)width, (float)-height}));
    glm_look(cam_pos, cam_dir, cam_up, glob_uniform.view);
    glm_perspective(glm_rad(67.5f), bgl_get_viewport_aspect_ratio(bgl), 0.1f, 100.0f, glob_uniform.proj);
    bgl_set_global_uniform(bgl, &glob_uniform, 0);

    glm_euler_angles(glob_uniform.view, cam_rot);
    cam_rot[0] = glm_deg(cam_rot[0]);
    cam_rot[1] = glm_deg(cam_rot[1]);
    cam_rot[2] = glm_deg(cam_rot[2]);

    return 0;
}

static void draw(bgl_instance bgl, float t) {
    {   // camera
        t *= 8;

        if (cam_rot_x)
            cam_rot[0] += cam_rot_x * t * 10;
        if (cam_rot_y)
            cam_rot[1] += cam_rot_y * t * 10;
        if (cam_rot_z)
            cam_rot[2] += cam_rot_z * t * 10;

        glm_vec3_copy((vec3){0, 0, -1}, cam_dir);
        glm_vec3_rotate(cam_dir, glm_rad(cam_rot[0]), (vec3){1, 0, 0});
        glm_vec3_rotate(cam_dir, glm_rad(cam_rot[1]), (vec3){0, 1, 0});
        glm_vec3_rotate(cam_dir, glm_rad(cam_rot[2]), (vec3){0, 0, 1});

        glm_vec3_copy((vec3){0, 1, 0}, cam_up);
        glm_vec3_rotate(cam_up, glm_rad(cam_rot[0]), (vec3){1, 0, 0});
        glm_vec3_rotate(cam_up, glm_rad(cam_rot[1]), (vec3){0, 1, 0});
        glm_vec3_rotate(cam_up, glm_rad(cam_rot[2]), (vec3){0, 0, 1});

        vec3 vt, vt2;
        if (cam_mov_x) {
            glm_vec3_fill(vt2, cam_mov_x * t);

            glm_vec3_crossn(cam_dir, cam_up, vt);
            glm_vec3_mul(vt, vt2, vt);
            glm_vec3_add(cam_pos, vt, cam_pos);
        }
        if (cam_mov_y) {
            glm_vec3_fill(vt2, cam_mov_y * t);

            glm_vec3_mul(cam_up, vt2, vt);
            glm_vec3_add(cam_pos, vt, cam_pos);
        }
        if (cam_mov_z) {
            glm_vec3_fill(vt2, cam_mov_z * t);

            glm_vec3_mul(cam_dir, vt2, vt);
            glm_vec3_add(cam_pos, vt, cam_pos);
        }

        glm_look(cam_pos, cam_dir, cam_up, glob_uniform.view);
    }

    bgl_draw_index_buffers(bgl, BGL_TRIANGLES);
    bgl_swap_buffers(bgl);
}

int main(int argc, const char **argv) {
    if (argc != 2) {
        fputs("obj file path is not specified", stderr);
        exit(EXIT_FAILURE);
    }

    char title[64];
    char title_fmt[] = "BGL Scene | FPS: %.01f";

    bgl_instance bgl = bgl_init();
    if (!bgl)
        exit(EXIT_FAILURE);

    if (!bgl_create_window(bgl, width, height, NULL)) {
        fprintf(stderr, "no window\n");
        bgl_terminate(bgl);
        exit(EXIT_FAILURE);
    }

    if (init(bgl, argv[1])) {
        fprintf(stderr, "init error: %s\n", strerror(errno));
        bgl_terminate(bgl);
        exit(EXIT_FAILURE);
    }

    uint64_t t = 0, tt;
    int64_t d;
    double fps;
    int64_t delay = (int64_t)bgl_get_timer_freq(bgl) / fps_lim;

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
        draw(bgl, (float)(1.0 / fps));
    }

    bgl_destroy_window(bgl);
    bgl_terminate(bgl);

    return 0;
}
