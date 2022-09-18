/*
 * Copyright (c) 2022 Alexander Baskikh
 *
 * MIT License (MIT), http://opensource.org/licenses/MIT
 * Full license can be found in the LICENSE file
 */

#ifndef BGL_WIN32_PLATFORM_H
#define BGL_WIN32_PLATFORM_H

#include <windows.h>
#include <winuser.h>


struct bgl_platform {
    HINSTANCE instance;
    ATOM main_window_class;

    short keycodes[512];
    unsigned scancodes[BGL_KEY_MAX];
};


struct bgl_platform_window {
    HWND window;

    int width;
    int height;

    // renderers
    struct {
    } base;
};


///////////////////////////////////////////////////////////////////////////////

WCHAR *bgl_create_wstr_from_utf8(const char *s);

#endif // BGL_WIN32_PLATFORM_H
