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


int width = 600;
int height = 600;

void init(bgl_instance bgl) {
    vertex triangle[] = {
            {0.0f, 1.5f},
            {1.5f, -1.5f},
            {-1.5f, 0.0f},
    };
    vindex indices[] = {
            {0}, {1}, {2},
    };

    vec4s color = {.g = 0.717647058823f, .b = 0.921568627451f, .a = 1.0f};
    for (int i = 0; i < sizeof(indices) / sizeof(*indices); ++i)
        glm_vec4_copy(color.raw, indices[i].color);

    for (int i = 0; i < sizeof(triangle) / sizeof(*triangle); ++i)
        glm_vec4_copy(color.raw, triangle[i].color);

    int vbuf = bgl_create_vertex_buffer(bgl, triangle, sizeof(triangle) / sizeof(*triangle), 0);
    bgl_create_index_buffer(bgl, vbuf, indices, sizeof(indices) / sizeof(*indices), 0);

    bgl_set_viewport(bgl, &(bgl_viewport){0, (float)height, (float)width, (float)-height});
}

void draw(bgl_instance bgl, float t) {

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
