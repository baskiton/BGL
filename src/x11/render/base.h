/*
 * Copyright (c) 2022 Alexander Baskikh
 *
 * MIT License (MIT), http://opensource.org/licenses/MIT
 * Full license can be found in the LICENSE file
 */

#ifndef BGL_X11_RENDER_BASE_H
#define BGL_X11_RENDER_BASE_H

#include "internal.h"


int init_x11_base_render(bgl_instance bgl, Visual **visual, int *depth);
int create_x11_base_render(bgl_instance bgl, Visual *visual, int depth);
void destroy_x11_base_render(bgl_instance bgl);

#endif // BGL_X11_RENDER_BASE_H
