/*
 * Copyright (c) 2022 Alexander Baskikh
 *
 * MIT License (MIT), http://opensource.org/licenses/MIT
 * Full license can be found in the LICENSE file
 */

#ifndef BGL_WIN32_PLATFORM_H
#define BGL_WIN32_PLATFORM_H

struct bgl_platform {
};


struct bgl_platform_window {
    int width;
    int height;

    // renderers
    struct {
    } base;
};

#endif // BGL_WIN32_PLATFORM_H
