/*
 * Copyright (c) 2022 Alexander Baskikh
 *
 * MIT License (MIT), http://opensource.org/licenses/MIT
 * Full license can be found in the LICENSE file
 */

#ifndef BGL_X11_PLATFORM_H
#define BGL_X11_PLATFORM_H


#include <X11/Xatom.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xresource.h>
//#include <X11/keysym.h>


static const char *atom_names[] = {
        "UTF8_STRING",
        "WM_PROTOCOLS",
        "WM_DELETE_WINDOW",
        "_NET_WM_PING",
        "_NET_WM_NAME",
        "_NET_WM_ICON_NAME",
        "_NET_WM_WINDOW_TYPE",
        "_NET_WM_WINDOW_TYPE_NORMAL",
};


struct bgl_platform {
    Display *display;
    int screen;
    Window root;
    XContext context;

    short keycodes[256];

    int empty_evt_pipe[2];
# define eevt_rd empty_evt_pipe[0]
# define eevt_wr empty_evt_pipe[1]

    // Window manager atoms
    struct {
        Atom
                UTF8_STRING,
                WM_PROTOCOLS,
                WM_DELETE_WINDOW,
                NET_WM_PING,
                NET_WM_NAME,
                NET_WM_ICON_NAME,
                NET_WM_WINDOW_TYPE,
                NET_WM_WINDOW_TYPE_NORMAL;
    } atoms;
};


struct bgl_platform_window {
    Window parent;
    Window window;
    Colormap cmap;

    int width;
    int height;

    // renderers
    struct {
        XImage *ximg;
        GC gc;
        void *buffer;
    } base;
};


#endif // BGL_X11_PLATFORM_H
