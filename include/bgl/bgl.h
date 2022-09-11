/*
 * Copyright (c) 2022 Alexander Baskikh
 *
 * MIT License (MIT), http://opensource.org/licenses/MIT
 * Full license can be found in the LICENSE file
 */

#ifndef BGL_BGL_H
#define BGL_BGL_H

#include <stdint.h>
#include <stddef.h>

#include <bgl/bglm.h>


#if defined _WIN32 || defined __CYGWIN__
# define BGL_API __declspec(dllexport)
#else
# if __GNUC__ >= 4
#  define BGL_API __attribute__((visibility("default")))
# else
#  define BGL_API
# endif
#endif


typedef enum {
    BGL_BASE_RENDER_API = 0x2000,
} bgl_render_api;

typedef enum {
    BGL_POINTS = 1,
    BGL_LINES,
    BGL_LINES_STRIP,
    BGL_LINES_LOOP,
    BGL_TRIANGLES,
    BGL_TRIANGLES_STRIP,
    BGL_TRIANGLES_FAN,
} bgl_drawing_modes;

typedef enum {
    BGL_KEY_UNKNOWN = -1,

    // printable keys
    BGL_KEY_SPACE,
    BGL_KEY_APOSTROPHE, // '
    BGL_KEY_COMMA,      // ,
    BGL_KEY_MINUS,      // -
    BGL_KEY_PERIOD,     // .
    BGL_KEY_SLASH,      // /
    BGL_KEY_0,
    BGL_KEY_1,
    BGL_KEY_2,
    BGL_KEY_3,
    BGL_KEY_4,
    BGL_KEY_5,
    BGL_KEY_6,
    BGL_KEY_7,
    BGL_KEY_8,
    BGL_KEY_9,
    BGL_KEY_SEMICOLON,  // ;
    BGL_KEY_EQUAL,      // =
    BGL_KEY_A,
    BGL_KEY_B,
    BGL_KEY_C,
    BGL_KEY_D,
    BGL_KEY_E,
    BGL_KEY_F,
    BGL_KEY_G,
    BGL_KEY_H,
    BGL_KEY_I,
    BGL_KEY_J,
    BGL_KEY_K,
    BGL_KEY_L,
    BGL_KEY_M,
    BGL_KEY_N,
    BGL_KEY_O,
    BGL_KEY_P,
    BGL_KEY_Q,
    BGL_KEY_R,
    BGL_KEY_S,
    BGL_KEY_T,
    BGL_KEY_U,
    BGL_KEY_V,
    BGL_KEY_W,
    BGL_KEY_X,
    BGL_KEY_Y,
    BGL_KEY_Z,
    BGL_KEY_LEFT_BRACKET,   // [
    BGL_KEY_BACKSLASH,
    BGL_KEY_RIGHT_BRACKET,  // ]
    BGL_KEY_GRAVE_ACCENT,   // `
    BGL_KEY_WORLD_1,    // non-US #1
    BGL_KEY_WORLD_2,    // non-US #2

    // function keys
    BGL_KEY_ESCAPE,
    BGL_KEY_ENTER,
    BGL_KEY_TAB,
    BGL_KEY_BACKSPACE,
    BGL_KEY_INSERT,
    BGL_KEY_DELETE,
    BGL_KEY_RIGHT,
    BGL_KEY_LEFT,
    BGL_KEY_DOWN,
    BGL_KEY_UP,
    BGL_KEY_PAGE_UP,
    BGL_KEY_PAGE_DOWN,
    BGL_KEY_HOME,
    BGL_KEY_END,
    BGL_KEY_CAPS_LOCK,
    BGL_KEY_SCROLL_LOCK,
    BGL_KEY_NUM_LOCK,
    BGL_KEY_PRINT_SCREEN,
    BGL_KEY_PAUSE,
    BGL_KEY_F1,
    BGL_KEY_F2,
    BGL_KEY_F3,
    BGL_KEY_F4,
    BGL_KEY_F5,
    BGL_KEY_F6,
    BGL_KEY_F7,
    BGL_KEY_F8,
    BGL_KEY_F9,
    BGL_KEY_F10,
    BGL_KEY_F11,
    BGL_KEY_F12,
    BGL_KEY_F13,
    BGL_KEY_F14,
    BGL_KEY_F15,
    BGL_KEY_F16,
    BGL_KEY_F17,
    BGL_KEY_F18,
    BGL_KEY_F19,
    BGL_KEY_F20,
    BGL_KEY_F21,
    BGL_KEY_F22,
    BGL_KEY_F23,
    BGL_KEY_F24,
    BGL_KEY_F25,
    BGL_KEY_KP_0,
    BGL_KEY_KP_1,
    BGL_KEY_KP_2,
    BGL_KEY_KP_3,
    BGL_KEY_KP_4,
    BGL_KEY_KP_5,
    BGL_KEY_KP_6,
    BGL_KEY_KP_7,
    BGL_KEY_KP_8,
    BGL_KEY_KP_9,
    BGL_KEY_KP_DECIMAL,
    BGL_KEY_KP_DIVIDE,
    BGL_KEY_KP_MULTIPLY,
    BGL_KEY_KP_SUBTRACT,
    BGL_KEY_KP_ADD,
    BGL_KEY_KP_ENTER,
    BGL_KEY_KP_EQUAL,
    BGL_KEY_LEFT_SHIFT,
    BGL_KEY_LEFT_CONTROL,
    BGL_KEY_LEFT_ALT,
    BGL_KEY_LEFT_SUPER,
    BGL_KEY_RIGHT_SHIFT,
    BGL_KEY_RIGHT_CONTROL,
    BGL_KEY_RIGHT_ALT,
    BGL_KEY_RIGHT_SUPER,
    BGL_KEY_MENU,

    BGL_KEY_MAX
} bgl_key;

typedef enum {
    BGL_MOD_SHIFT = 1 << 0,
    BGL_MOD_CAPS = 1 << 1,
    BGL_MOD_CTRL = 1 << 2,
    BGL_MOD_ALT = 1 << 3,
    BGL_MOD_NUM = 1 << 4,
    BGL_MOD_SUPER = 1 << 5,
} bgl_key_mods;

typedef enum {
    BGL_RELEASE = 0,
    BGL_PRESS,
    BGL_REPEATE,
} bgl_key_action;


#define BGL_DEFINE_HANDLE(object) typedef struct object* object
#define BGL_DEFINE_STRUCT(object) typedef struct object object

BGL_DEFINE_HANDLE(bgl_instance);
BGL_DEFINE_STRUCT(bgl_viewport);

typedef void (*bgl_close_window_fn)(bgl_instance bgl);
typedef void (*bgl_key_fn)(bgl_instance bgl, bgl_key key, unsigned scancode, bgl_key_action action, bgl_key_mods mods);


struct bgl_viewport {
    float x;
    float y;
    float width;
    float height;
};


BGL_API bgl_instance bgl_init();
BGL_API void bgl_terminate(bgl_instance bgl);

BGL_API double bgl_get_time(bgl_instance bgl);
BGL_API void bgl_set_time(bgl_instance bgl, double t);
BGL_API uint64_t bgl_get_timer(bgl_instance bgl);
BGL_API uint64_t bgl_get_timer_freq(bgl_instance bgl);

BGL_API int bgl_create_window(bgl_instance bgl, int width, int height, const char *title);
BGL_API void bgl_destroy_window(bgl_instance bgl);
BGL_API int bgl_window_should_close(bgl_instance bgl);
BGL_API void bgl_set_window_should_close(bgl_instance bgl, int val);
BGL_API void bgl_show_window(bgl_instance bgl);
BGL_API void bgl_set_window_title(bgl_instance bgl, const char *title);
BGL_API void bgl_swap_buffers(bgl_instance bgl);

BGL_API int bgl_set_window_close_callback(bgl_instance bgl, bgl_close_window_fn callback);
BGL_API int bgl_set_key_callback(bgl_instance bgl, bgl_key_fn callback);

BGL_API void bgl_poll_events(bgl_instance bgl);
BGL_API void bgl_wait_events(bgl_instance bgl);
BGL_API void bgl_wait_events_timeout(bgl_instance bgl, double timeout);
BGL_API void bgl_send_empty_event(bgl_instance bgl);

BGL_API int bgl_create_vertex_buffer(bgl_instance bgl, const vertex *vertices, int count, bgl_drawing_modes mode);
BGL_API void bgl_remove_vertex_buffer(bgl_instance bgl, int id);
BGL_API void bgl_clear_vertex_bufers(bgl_instance bgl);

BGL_API int bgl_create_index_buffer(bgl_instance bgl, int vbuf_id, const vindex *indices, int count, bgl_drawing_modes mode);
BGL_API void bgl_remove_index_buffer(bgl_instance bgl, int ibuf_id, int with_vbuf);
BGL_API void bgl_clear_index_buffers(bgl_instance bgl);

BGL_API void bgl_set_viewport(bgl_instance bgl, bgl_viewport *viewport);
BGL_API float bgl_get_viewport_aspect_ratio(bgl_instance bgl);
BGL_API void bgl_set_global_uniform(bgl_instance bgl, uniform *uniform, int mode);
BGL_API int bgl_bind_model_matrix(bgl_instance bgl, int vbuf_id, mat4 *model);

BGL_API void bgl_draw_vertex_buffers(bgl_instance bgl, bgl_drawing_modes mode);
BGL_API void bgl_draw_index_buffers(bgl_instance bgl, bgl_drawing_modes mode);

#endif // BGL_BGL_H
