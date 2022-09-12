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


int width = 600;
int height = 600;


int main() {
    char title[] = "BGL Window Test";

    bgl_instance bgl = bgl_init();
    if (!bgl) {
        fprintf(stderr, "init error\n");
        exit(EXIT_FAILURE);
    }

    if (!bgl_create_window(bgl, width, height, NULL)) {
        fprintf(stderr, "no window\n");
        bgl_terminate(bgl);
        exit(EXIT_FAILURE);
    }
    bgl_set_window_title(bgl, title);

    while (!bgl_window_should_close(bgl)) {
        bgl_wait_events(bgl);
    }

    bgl_destroy_window(bgl);
    bgl_terminate(bgl);

    return 0;
}
