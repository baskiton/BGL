/*
 * Copyright (c) 2022 Alexander Baskikh
 *
 * MIT License (MIT), http://opensource.org/licenses/MIT
 * Full license can be found in the LICENSE file
 */

#include <errno.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "internal.h"
#include "base.h"


#define SWAP(a, b) (((a) ^= (b)), ((b) ^= (a)), ((a) ^= (b)))
#define VEC2ARGB(color) (                               \
        (uint8_t)(((vec4s *)color)->a * 255) << 24      \
        | (uint8_t)(((vec4s *)color)->r * 255) << 16    \
        | (uint8_t)(((vec4s *)color)->g * 255) << 8     \
        | (uint8_t)(((vec4s *)color)->b * 255))


static void swap_buffers(bgl_instance bgl) {
    XPutImage(bgl->platform.display, bgl->window->platform.window,
              bgl->window->platform.base.gc, bgl->window->platform.base.ximg,
              0, 0, 0, 0,
              bgl->window->platform.width, bgl->window->platform.height);
    memset(bgl->window->platform.base.buffer, 0, bgl->window->platform.width * bgl->window->platform.height * sizeof(uint32_t));
}

static void draw_pixel(bgl_instance bgl, const ivec3 v, const vec4 color) {
//    if (v->x >= 0 && v->x < bgl->window->platform.width && v->y >= 0 && v->y < bgl->window->platform.height)
        XPutPixel(bgl->window->platform.base.ximg, v[0], v[1], VEC2ARGB(color));
}

static void write_hline(bgl_instance bgl, int x1, int x2, int y, uint32_t color) {
//    if (y < 0 || y >= bgl->window->platform.height)
//        return;

//    int width = bgl->window->platform.width - 1;

    if (x1 > x2)
        SWAP(x1, x2);

//    if (x1 < 0)
//        x1 = 0;
//    else if (unlikely(x1 > width))
//        return;

//    if (x2 > width)
//        x2 = width;
//    else if (unlikely(x2 < 0))
//        return;

    uint32_t *buf = (uint32_t *)bgl->window->platform.base.buffer + y * bgl->window->platform.width;
    for (uint32_t *p = &buf[x1]; p <= &buf[x2]; ++p)
        *p = color;
}

static void write_vline(bgl_instance bgl, int x, int y1, int y2, uint32_t color) {
//    if (x < 0 || x >= bgl->window->platform.width)
//        return;

//    int height = bgl->window->platform.height - 1;

    if (y1 > y2)
        SWAP(y1, y2);

//    if (y1 < 0)
//        y1 = 0;
//    else if (unlikely(y1 > height))
//        return;

//    if (y2 > height)
//        y2 = height;
//    else if (unlikely(y2 < 0))
//        return;

    for (int y = y1; y <= y2; ++y)
        XPutPixel(bgl->window->platform.base.ximg, x, y, color);
}

/*!
 * @brief Write a line. Bresenham's algorithm
 */
static void write_line(bgl_instance bgl, const ivec3 a, const ivec3 b, uint32_t color) {
    uint32_t *buf = bgl->window->platform.base.buffer;
    int width = bgl->window->platform.width;
//    int height = bgl->window->platform.height;
    int ax = a[0], ay = a[1], bx = b[0], by = b[1];

    if (ay == by) {
        write_hline(bgl, ax, bx, ay, color);
        return;
    } else if (ax == bx) {
        write_vline(bgl, ax, ay, by, color);
        return;
    }

    int dx, dy, err, ystep;
    int angle = abs(by - ay) > abs(bx - ax);
    if (angle) {
        SWAP(ax, ay);
        SWAP(bx, by);
    }

    if (ax > bx) {
        SWAP(ax, bx);
        SWAP(ay, by);
    }

    dx = bx - ax;
    dy = abs(by - ay);
    err = dx / 2;
    ystep = (ay < by) ? 1 : -1;

    for (; ax <= bx ; ++ax) {
        // check that the pixels are within the screen
        if (angle) {
//            XPutPixel(bgl->window->platform.base.ximg, ay, ax, color);
//            if ((ax >= 0) && (ax < height)
//                    && (ay >= 0) && (ay < width)) {
                buf[ax * width + ay] = color;
//                XPutPixel(bgl->window->platform.base.ximg, ay, ax, color);
//            }
        } else {
//            XPutPixel(bgl->window->platform.base.ximg, ax, ay, color);
//            if ((ay >= 0) && (ay < height)
//                    && (ax >= 0) && (ax < width)) {
                buf[ay * width + ax] = color;
//                XPutPixel(bgl->window->platform.base.ximg, ax, ay, color);
//            }
        }
        err -= dy;
        if (err < 0) {
            ay += ystep;
            err += dx;
        }
    }
}

static void draw_line(bgl_instance bgl, const ivec3 a, const ivec3 b, const vec4 color) {
    write_line(bgl, a, b, VEC2ARGB(color));
}

static void draw_triangle(bgl_instance bgl, const ivec3 a, const ivec3 b, const ivec3 c, const vec4 color) {
    uint32_t argb = VEC2ARGB(color);

    write_line(bgl, a, b, argb);
    write_line(bgl, b, c, argb);
    write_line(bgl, c, a, argb);
}

static void draw_fill_triangle(bgl_instance bgl, const ivec3 a, const ivec3 b, const ivec3 c, const vec4 color) {
//    int width = bgl->window->platform.width - 1;
//    int height = bgl->window->platform.height - 1;
    uint32_t argb = VEC2ARGB(color);
    int ax = a[0], ay = a[1], bx = b[0], by = b[1], cx = c[0], cy = c[1];

    int dx_c, dx_b, dx_a, dy_c, dy_b, dy_a;
    int d_s, d_e;   // delta for start/end drawing line
    int ls_x, l_y, le_x;    // coordinates for drawing line

    // sort coordinates by order (ay <= by <= cy)
    if (ay > by) {
        SWAP(ay, by);
        SWAP(ax, bx);
    }
    if (by > cy) {
        SWAP(by, cy);
        SWAP(bx, cx);
    }
    if (ay > by) {
        SWAP(ay, by);
        SWAP(ax, bx);
    }

    // if triangle - horizontal line
    if (ay == cy) {
        // sort x-coordinates from smallest to largest
        ls_x = le_x = ax;
        if (bx < ls_x)
            ls_x = bx;
        else if (bx > le_x)
            le_x = bx;
        if (cx < ls_x)
            ls_x = cx;
        else if (cx > le_x)
            le_x = cx;

        write_hline(bgl, ls_x, le_x, ay, argb);
        return;
    }

    // if triangle - vertical line
    if ((ax == bx) && (ax == cx)) {
        write_vline(bgl, ax, ay, cy, argb);
        return;
    }

    dx_c = bx - ax;
    dy_c = by - ay;
    dx_b = cx - ax;
    dy_b = cy - ay;
    dx_a = cx - bx;
    dy_a = cy - by;

//    ls_x = le_x = ax;
    l_y = ay;

    d_s = d_e = 0;

    // upper part of triangle
    for (; l_y < by; ++l_y) {
        ls_x = ax + (int)lround((double)d_s / dy_c);
        le_x = ax + (int)lround((double)d_e / dy_b);
        d_s += dx_c;
        d_e += dx_b;
        if (ls_x > le_x)
            SWAP(ls_x, le_x);
//        if (ls_x < 0)
//            ls_x = 0;
//        if (le_x > width)
//            le_x = width;
//        if ((l_y >= 0) && (l_y <= height))
        write_hline(bgl, ls_x, le_x, l_y, argb);
    }

    // lower part of triangle
    // division by 0 protection
    if (dy_a && dy_b) {
        d_s = dx_a * (l_y - by);
        d_e = dx_b * (l_y - ay);
        for (; l_y <= cy; ++l_y) {
            ls_x = bx + (int)lround((double)d_s / dy_a);
            le_x = ax + (int)lround((double)d_e / dy_b);
            d_s += dx_a;
            d_e += dx_b;
            if (ls_x > le_x)
                SWAP(ls_x, le_x);
//            if (ls_x < 0)
//                ls_x = 0;
//            if (le_x > width)
//                le_x = width;
//            if ((l_y >= 0) && (l_y <= height))
            write_hline(bgl, ls_x, le_x, l_y, argb);
        }
    } else {
        ls_x = bx;
        le_x = cx;
        if (ls_x > le_x)
            SWAP(ls_x, le_x);
//        if (ls_x < 0)
//            ls_x = 0;
//        if (le_x > width)
//            le_x = width;
//        if ((l_y >= 0) && (l_y <= height))
        write_hline(bgl, ls_x, le_x, l_y, argb);
    }
}

///////////////////////////////////////////////////////////////////////////////

int init_x11_base_render(bgl_instance bgl, Visual **visual, int *depth) {
    XVisualInfo *res;
    XVisualInfo tmp = {
            .screen = bgl->platform.screen,
//            .depth = 32,
    };
    int cnt = 0;

    if (!(res = XGetVisualInfo(bgl->platform.display, VisualScreenMask
//            | VisualDepthMask
            , &tmp, &cnt))) {
        fprintf(stderr, "Failed to get visual info\n");
        return false;
    }

    *visual = res->visual;
    *depth = res->depth;

    XFree(res);

    bgl->dev.destroy_render = destroy_x11_base_render;
    bgl->dev.swap_buffers = swap_buffers;

    bgl->dev.draw_pixel = draw_pixel;
    bgl->dev.draw_line = draw_line;
    bgl->dev.draw_triangle = draw_triangle;
    bgl->dev.draw_fill_triangle = draw_fill_triangle;

    return true;
}

int create_x11_base_render(bgl_instance bgl, Visual *visual, int depth) {
    size_t fb_size = bgl->window->platform.width * bgl->window->platform.height * 4;    // ARGB
    typeof(bgl->window->platform.base) *render = &bgl->window->platform.base;

    destroy_x11_base_render(bgl);

    if (!(render->buffer = aligned_alloc(32, fb_size))) {
        fprintf(stderr, "Failed to create render: framebuffer: %s\n", strerror(errno));
        return false;
    }
    for (int i = 0; i < bgl->window->platform.width * bgl->window->platform.height; ++i)
        ((uint32_t *)render->buffer)[i] = 0xFF000000;

    render->ximg = XCreateImage(bgl->platform.display,
                                visual, depth, ZPixmap,
                                0, render->buffer,
                                bgl->window->platform.width, bgl->window->platform.height, 32, 0);

    render->gc = XCreateGC(bgl->platform.display, bgl->window->platform.window, 0, NULL);

    return true;
}

void destroy_x11_base_render(bgl_instance bgl) {
    typeof(bgl->window->platform.base) *render = &bgl->window->platform.base;

    if (render->buffer)
        free(render->buffer);

    if (render->ximg) {
        render->ximg->data = NULL;
        XDestroyImage(render->ximg);
    }

    if (render->gc)
        XFreeGC(bgl->platform.display, render->gc);

    memset(render, 0, sizeof(*render));
}
