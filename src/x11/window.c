/*
 * Copyright (c) 2022 Alexander Baskikh
 *
 * MIT License (MIT), http://opensource.org/licenses/MIT
 * Full license can be found in the LICENSE file
 */

#define _GNU_SOURCE

#include <errno.h>
#include <poll.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "internal.h"
#include "render/base.h"


#define S2MS 1e3
#define S2NS 1e9

static int create_x11_window(bgl_instance bgl, const bgl_window_cfg *w_cfg, Visual *visual, int depth) {
    bgl_platform_window *x11w = &bgl->window->platform;

    x11w->cmap = XCreateColormap(bgl->platform.display, bgl->platform.root, visual, AllocNone);

    XSetWindowAttributes attrs = {0};
    attrs.colormap = x11w->cmap;
    attrs.event_mask = KeyPressMask | KeyReleaseMask
                       | ButtonPressMask | ButtonReleaseMask
                       | StructureNotifyMask
                       | ExposureMask
                       | VisibilityChangeMask
                       | EnterWindowMask | LeaveWindowMask
                       | PropertyChangeMask
//            | PointerMotionMask
//            | FocusChangeMask
            ;

    // TODO: Grab Error Handler?

    x11w->parent = bgl->platform.root;
    x11w->window = XCreateWindow(bgl->platform.display,
                                 bgl->platform.root,
                                 0, 0,
                                 w_cfg->width, w_cfg->height,
                                 0, depth, InputOutput,
                                 visual,
                                 CWBorderPixel | CWColormap | CWEventMask,
                                 &attrs);

    // TODO: Release Error Handler?

    if (!x11w->window) {
        fprintf(stderr, "X11: Failed to create window\n");
        return false;
    }

//    XSaveContext(bgl->platform.display, x11w->window, bgl->platform.context, (XPointer)w);

    // TODO: check NET_WM_STATE and change window attrs (maximized. floating...)

    // TODO: declare PID with NET_WM_PID

    Atom protos[] = {
            bgl->platform.atoms.WM_DELETE_WINDOW,
            bgl->platform.atoms.NET_WM_PING,
    };
    XSetWMProtocols(bgl->platform.display, x11w->window, protos, sizeof(protos) / sizeof(Atom));

    if (bgl->platform.atoms.NET_WM_WINDOW_TYPE && bgl->platform.atoms.NET_WM_WINDOW_TYPE_NORMAL) {
        XChangeProperty(bgl->platform.display, x11w->window,
                        bgl->platform.atoms.NET_WM_WINDOW_TYPE,
                        XA_ATOM, 32, PropModeReplace,
                        (unsigned char *)&bgl->platform.atoms.NET_WM_WINDOW_TYPE_NORMAL, 1);
    }

    {
        XClassHint *hint = XAllocClassHint();
        char *res_name = getenv("RESOURCE_NAME");
        if (res_name && *res_name)
            hint->res_name = res_name;
        else if (w_cfg->title && *w_cfg->title)
            hint->res_name = (char *)w_cfg->title;
        else
            hint->res_name = "BGL-Window";

        if (w_cfg->title && *w_cfg->title)
            hint->res_class = (char *)w_cfg->title;
        else
            hint->res_class = "BGL-Window";

        XSetClassHint(bgl->platform.display, x11w->window, hint);
        XFree(hint);
    }

    {
        XSizeHints *hints = XAllocSizeHints();

        if (!w_cfg->resizable) {
            hints->flags |= PMinSize | PMaxSize;
            hints->min_width = hints->max_width = w_cfg->width;
            hints->min_height = hints->max_height = w_cfg->height;
        }

        hints->flags |= PWinGravity;
        hints->win_gravity = StaticGravity;

        XSetWMNormalHints(bgl->platform.display, x11w->window, hints);
        XFree(hints);
    }

    set_platform_window_title(bgl, w_cfg->title ? : "BGL-Window");
    // TODO: set pos and size
    x11w->width = w_cfg->width;
    x11w->height = w_cfg->height;

    return true;
}

static int poll_posix(bgl_instance bgl, struct pollfd* fds, nfds_t count, const double* timeout) {
    while (1) {
        if (timeout) {
            double t = *timeout;
            const uint64_t base = get_platform_timer_value(bgl);

#if defined(__linux__) || defined(__FreeBSD__) || defined(__OpenBSD__) || defined(__CYGWIN__)
            const time_t seconds = (time_t)t;
            const long nanoseconds = (long)((t - (double)seconds) * S2NS);
            const struct timespec ts = {seconds, nanoseconds};
            const int result = ppoll(fds, count, &ts, NULL);
#elif defined(__NetBSD__)
            const time_t seconds = (time_t)t;
            const long nanoseconds = (long)((t - (double)seconds) * S2NS);
            const struct timespec ts = {seconds, nanoseconds};
            const int result = pollts(fds, count, &ts, NULL);
#else
            const int milliseconds = (int)(t * S2MS);
            const int result = poll(fds, count, milliseconds);
#endif
            const int error = errno; // clock_gettime may overwrite our error

            t -= (double)(get_platform_timer_value(bgl) - base) /
                 (double)get_platform_timer_freq(bgl);

            if (result > 0)
                return true;
            else if (result == -1 && error != EINTR && error != EAGAIN || t <= 0.0)
                return false;
        } else {
            const int result = poll(fds, count, -1);
            if (result > 0)
                return true;
            else if (result == -1 && errno != EINTR && errno != EAGAIN)
                return false;
        }
    }
}

static int wait_any_event(bgl_instance bgl, double *t) {
    struct pollfd fds[2] = {
            {bgl->platform.eevt_rd, POLLIN},
            {ConnectionNumber(bgl->platform.display), POLLIN},
    };

    while (!XPending(bgl->platform.display)) {
        if (!poll_posix(bgl, fds, 2, t))
            return false;

        if (fds[0].revents & POLLIN || fds[1].revents & POLLIN)
            return true;
    }
    return true;
}

static int translate_key(bgl_instance bgl, unsigned keycode) {
    if (keycode > 255)
        return BGL_KEY_UNKNOWN;

    return bgl->platform.keycodes[keycode];
}

static int translate_state(unsigned state) {
    int mods = 0;

    if (state & ShiftMask)
        mods |= BGL_MOD_SHIFT;
    if (state & LockMask)
        mods |= BGL_MOD_CAPS;
    if (state & ControlMask)
        mods |= BGL_MOD_CTRL;
    if (state & Mod1Mask)
        mods |= BGL_MOD_ALT;
    if (state & Mod2Mask)
        mods |= BGL_MOD_NUM;
    if (state & Mod4Mask)
        mods |= BGL_MOD_SUPER;

    return mods;
}

static void process_event(bgl_instance bgl, XEvent *evt) {
    unsigned keycode = 0;

    if (evt->type == KeyPress || evt->type == KeyRelease)
        keycode = evt->xkey.keycode;

    Bool filtered = XFilterEvent(evt, None);

    // TODO:
    switch (evt->type) {
    case KeyPress:
    {
        int key = translate_key(bgl, keycode);
        int mods = translate_state(evt->xkey.state);

        Time diff = evt->xkey.time - bgl->window->keys[keycode].time;
        if (diff == evt->xkey.time || (diff > 0 && diff < ((Time)1 << 31))) {
            if (keycode)
                input_key(bgl, key, keycode, BGL_PRESS, mods);
            bgl->window->keys[keycode].time = evt->xkey.time;
        }

        return;
    }

    case KeyRelease:
    {
        int key = translate_key(bgl, keycode);
        int mods = translate_state(evt->xkey.state);

        {
            if (XEventsQueued(bgl->platform.display, QueuedAfterReading)) {
                XEvent next;
                XPeekEvent(bgl->platform.display, &next);

                if (next.type == KeyPress
                        && next.xkey.window == evt->xkey.window
                        && next.xkey.keycode == keycode
                        && (next.xkey.time - evt->xkey.time) < 20)
                    return;
            }
        }

        input_key(bgl, key, keycode, BGL_RELEASE, mods);

        return;
    }

    case ButtonPress:
    case ButtonRelease:
        return;
    case ClientMessage:
        if (filtered)
            return;

        if (evt->xclient.message_type == bgl->platform.atoms.WM_PROTOCOLS) {
            Atom proto = evt->xclient.data.l[0];
            if (proto == None)
                return;

            if (proto == bgl->platform.atoms.WM_DELETE_WINDOW)
                input_window_close_request(bgl);
            else if (proto == bgl->platform.atoms.NET_WM_PING) {
                XEvent resp = *evt;
                resp.xclient.window = bgl->platform.root;
                XSendEvent(bgl->platform.display,
                           bgl->platform.root,
                           False,
                           SubstructureNotifyMask | SubstructureRedirectMask,
                           &resp);
            }
        }

        return;
    case ReparentNotify:
        bgl->window->platform.parent = evt->xreparent.parent;
        return;
    case EnterNotify:
    case LeaveNotify:
    case Expose:    // window content needs updating/refreshing
        return;
    case ConfigureNotify:
        if (evt->xconfigure.width != bgl->window->platform.width
                || evt->xconfigure.height != bgl->window->platform.height) {
            bgl->window->platform.width = evt->xconfigure.width;
            bgl->window->platform.height = evt->xconfigure.height;

//            _bgl_set_base_framebuffer_size(w, w->platform.width, w->platform.height);
        }
        return;
    case DestroyNotify:
    default:
        return;
    }
}

///////////////////////////////////////////////////////////////////////////////

int create_platform_window(bgl_instance bgl, const bgl_window_cfg *w_cfg, const bgl_fb_cfg *fb_cfg, const bgl_render_cfg *rndr_cfg) {
    Visual *visual = NULL;
    int depth = 0;

    if (rndr_cfg->api == BGL_BASE_RENDER_API) {
        if (!init_x11_base_render(bgl, &visual, &depth))
            return false;
    } else {
        fprintf(stderr, "Invalid render API: 0x%04X\n", rndr_cfg->api);
        return false;
    }

    if (!visual) {
        visual = DefaultVisual(bgl->platform.display, bgl->platform.screen);
        depth = DefaultDepth(bgl->platform.display, bgl->platform.screen);
    }

    if (!create_x11_window(bgl, w_cfg, visual, depth))
        return false;

    if (rndr_cfg->api == BGL_BASE_RENDER_API) {
        if (!create_x11_base_render(bgl, visual, depth))
            return false;
    } else {
        fprintf(stderr, "Invalid render API: 0x%04X\n", rndr_cfg->api);
        return false;
    }

    if (w_cfg->visible)
        show_platform_window(bgl);

    XFlush(bgl->platform.display);

    bgl->dev.swap_buffers(bgl);

    return true;
}

void destroy_platform_window(bgl_instance bgl) {
    bgl_platform_window *x11w = &bgl->window->platform;

    if (bgl->dev.destroy_render)
        bgl->dev.destroy_render(bgl);

    if (x11w->window) {
        XDeleteContext(bgl->platform.display, x11w->window, bgl->platform.context);
        XUnmapWindow(bgl->platform.display, x11w->window);
        XDestroyWindow(bgl->platform.display, x11w->window);
        x11w->window = (Window)0;
    }

    if (x11w->cmap) {
        XFreeColormap(bgl->platform.display, x11w->cmap);
        x11w->cmap = (Colormap)0;
    }

    XFlush(bgl->platform.display);
}

void show_platform_window(bgl_instance bgl) {
    if (!is_visible_platform_window(bgl))
        XMapWindow(bgl->platform.display, bgl->window->platform.window);
}

void set_platform_window_title(bgl_instance bgl, const char *title) {
    // TODO: utf-8 support?

    XChangeProperty(bgl->platform.display, bgl->window->platform.window,
                    bgl->platform.atoms.NET_WM_NAME, bgl->platform.atoms.UTF8_STRING, 8,
                    PropModeReplace,
                    (const unsigned char *)title, (int)strlen(title));

    XChangeProperty(bgl->platform.display, bgl->window->platform.window,
                    bgl->platform.atoms.NET_WM_ICON_NAME, bgl->platform.atoms.UTF8_STRING, 8,
                    PropModeReplace,
                    (const unsigned char *)title, (int)strlen(title));

    XFlush(bgl->platform.display);
}

int is_visible_platform_window(bgl_instance bgl) {
    XWindowAttributes attr;

    XGetWindowAttributes(bgl->platform.display, bgl->window->platform.window, &attr);

    return attr.map_state == IsViewable;
}


/// window events

void poll_platform_window_events(bgl_instance bgl) {
    // TODO: drain empty events?

    XPending(bgl->platform.display);

    while (QLength(bgl->platform.display)) {
        XEvent evt;
        XNextEvent(bgl->platform.display, &evt);
        process_event(bgl, &evt);
    }

    XFlush(bgl->platform.display);
}

void wait_platform_window_events(bgl_instance bgl) {
    wait_any_event(bgl, NULL);
    poll_platform_window_events(bgl);
}

void wait_platform_window_events_timeout(bgl_instance bgl, double t) {
    wait_any_event(bgl, &t);
    poll_platform_window_events(bgl);
}

void send_platform_window_empty_event(bgl_instance bgl) {
    char d = 0;
    ssize_t x;
    do {
        x = write(bgl->platform.eevt_wr, &d, 1);
    } while (x != 1 && (x == -1 && errno == EINTR));
}
