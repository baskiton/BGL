/*
 * Copyright (c) 2022 Alexander Baskikh
 *
 * MIT License (MIT), http://opensource.org/licenses/MIT
 * Full license can be found in the LICENSE file
 */

#ifndef BGL_PLATFORM_H
#define BGL_PLATFORM_H

#include <stdint.h>


#if defined(_BGL_WIN32)
#  include "win32/platform.h"
#elif defined(_BGL_COCOA)
#  include "cocoa/platform.h"
#elif defined(_BGL_WAYLAND)
#  include "wl/platform.h"
#elif defined(_BGL_X11)
#  include "x11/platform.h"
#endif

#if defined(_WIN32) || defined(__APPLE__)
//# define BGL_LIB_PLATFORM_TIMER
//# define BGL_PLATFORM_TLS
#else
#include <time.h>
# define BGL_LIB_PLATFORM_TIMER clockid_t id;
//# define BGL_PLATFORM_TLS \
//        int allocated;    \
//        pthread_key_t key;
//# define BGL_PLATFORM_MUTEX   \
//        int allocated;        \
//        pthread_mutex_t mutex;
#endif


/// basic

int init_platform(bgl_instance bgl);
void terminate_platform(bgl_instance bgl);


/// time

void init_platform_timer(bgl_instance bgl);
uint64_t get_platform_timer_value(bgl_instance bgl);
uint64_t get_platform_timer_freq(bgl_instance bgl);


/// window

int create_platform_window(bgl_instance bgl, const bgl_window_cfg *w_cfg, const bgl_fb_cfg *fb_cfg,
                           const bgl_render_cfg *rndr_cfg);
void destroy_platform_window(bgl_instance bgl);
void show_platform_window(bgl_instance bgl);
void set_platform_window_title(bgl_instance bgl, const char *title);
int is_visible_platform_window(bgl_instance bgl);
//void get_platform_window_size(bgl_instance bgl, int *width, int *height);
//void set_platform_window_size(bgl_instance bgl, int width, int height);
//void get_platform_framebuffer_size(bgl_instance bgl, int *width, int *height);


/// window events

void poll_platform_window_events(bgl_instance bgl);
void wait_platform_window_events(bgl_instance bgl);
void wait_platform_window_events_timeout(bgl_instance bgl, double t);
void send_platform_window_empty_event(bgl_instance bgl);


#endif // BGL_PLATFORM_H
