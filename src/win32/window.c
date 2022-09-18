/*
 * Copyright (c) 2022 Alexander Baskikh
 *
 * MIT License (MIT), http://opensource.org/licenses/MIT
 * Full license can be found in the LICENSE file
 */

#include "internal.h"
#include "render/base.h"


static DWORD get_window_style(bgl_window w) {
    DWORD style = WS_CLIPSIBLINGS | WS_CLIPCHILDREN;

    // if window->monitor ...
    //     style |= WS_POPUP;
    // else
    {
        style |= WS_SYSMENU | WS_MINIMIZEBOX;
//        if (w->decorated) {
            style |= WS_CAPTION;
            if (w->resizable)
                style |= WS_MAXIMIZEBOX | WS_THICKFRAME;
//        } else
//        style |= WS_POPUP;
    }

    return style;
}

static DWORD get_window_ex_style(bgl_window w) {
    DWORD style = WS_EX_APPWINDOW;

    // if window->monitor || window->floating ...
    //     style |= WX_EX_TOPMOST;

    return style;
}

static LRESULT CALLBACK window_proc_callback(HWND h_wnd, UINT u_msg, WPARAM w_param, LPARAM l_param) {
    bgl_instance bgl = GetPropW(h_wnd, L"BGL");
    if (!bgl)
        return DefWindowProcW(h_wnd, u_msg, w_param, l_param);

    switch (u_msg) {
    case WM_CLOSE:
        input_window_close_request(bgl);
        return 0;

    default:
        break;
    }

    return DefWindowProcW(h_wnd, u_msg, w_param, l_param);
}

static int create_win32_window(bgl_instance bgl, const bgl_window_cfg *w_cfg) {
    bgl_platform_window *w32w = &bgl->window->platform;

    DWORD style = get_window_style(bgl->window);
    DWORD ex_style = get_window_ex_style(bgl->window);
    int x_pos, y_pos, full_width, full_height;

    if (!bgl->platform.main_window_class &&
            (!(bgl->platform.main_window_class = RegisterClassExW(
                    &(WNDCLASSEXW){
                        .cbSize = sizeof(WNDCLASSEXW),
                        .style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC,
                        .lpfnWndProc = window_proc_callback,
                        .hInstance = bgl->platform.instance,
                        .hCursor = LoadCursorW(NULL, IDC_ARROW),
                        .lpszClassName = L"BGL",
                        .hIcon = LoadImageW(NULL, IDI_APPLICATION, IMAGE_ICON, 0, 0, LR_DEFAULTSIZE | LR_SHARED),   // no icon
                    })))) {
        fprintf(stderr, "Win32: Failed to register window class\n");
        return false;
    }

    // if window->monitor ...
    // else
    {
        RECT rect = {0, 0, w_cfg->width, w_cfg->height};

        AdjustWindowRectEx(&rect, style, FALSE, ex_style);

        x_pos = CW_USEDEFAULT;
        y_pos = CW_USEDEFAULT;

        full_width = rect.right - rect.left;
        full_height = rect.bottom - rect.top;
    }

    WCHAR *w_title = bgl_create_wstr_from_utf8(w_cfg->title);
    if (!w_title)
        return false;

    w32w->window = CreateWindowExW(ex_style,
                                   MAKEINTATOM(bgl->platform.main_window_class),
                                   w_title,
                                   style,
                                   x_pos, y_pos,
                                   full_width, full_height,
                                   NULL,    // No parent window
                                   NULL,    // No window menu
                                   bgl->platform.instance,
                                   (LPVOID)w_cfg);

    free(w_title);

    if (!w32w->window) {
        fprintf(stderr, "Win32: Failed to create window\n");
        return false;
    }

    SetPropW(w32w->window, L"BGL", bgl);

    // if !window->monitor ...
    {
        RECT rect = {0, 0, w_cfg->width, w_cfg->height};
        WINDOWPLACEMENT wp = {sizeof(wp)};
        const HMONITOR mh = MonitorFromWindow(w32w->window, MONITOR_DEFAULTTONEAREST);

        AdjustWindowRectEx(&rect, style, FALSE, ex_style);

        GetWindowPlacement(w32w->window, &wp);
        OffsetRect(&rect, wp.rcNormalPosition.left - rect.left, wp.rcNormalPosition.top - rect.top);

        wp.rcNormalPosition = rect;
        wp.showCmd = SW_HIDE;
        SetWindowPlacement(w32w->window, &wp);
    }

    DragAcceptFiles(w32w->window, TRUE);

    // TODO: set pos and size
    w32w->width = w_cfg->width;
    w32w->height = w_cfg->height;

    return true;
}

static bgl_key_mods get_key_mods() {
    int mods = 0;

    if (GetKeyState(VK_SHIFT) & 0x8000)
        mods |= BGL_MOD_SHIFT;
    if (GetKeyState(VK_CAPITAL) & 1)
        mods |= BGL_MOD_CAPS;
    if (GetKeyState(VK_CONTROL) & 0x8000)
        mods |= BGL_MOD_CTRL;
    if (GetKeyState(VK_MENU) & 0x8000)
        mods |= BGL_MOD_ALT;
    if (GetKeyState(VK_NUMLOCK) & 1)
        mods |= BGL_MOD_NUM;
    if ((GetKeyState(VK_LWIN) | GetKeyState(VK_RWIN)) & 0x8000)
        mods |= BGL_MOD_SUPER;

    return mods;
}

///////////////////////////////////////////////////////////////////////////////

int create_platform_window(bgl_instance bgl,
                           const bgl_window_cfg *w_cfg,
                           const bgl_fb_cfg *fb_cfg,
                           const bgl_render_cfg *rndr_cfg) {
    if (!create_win32_window(bgl, w_cfg))
        return false;

    if (rndr_cfg->api == BGL_BASE_RENDER_API) {
//        if (!init_win32_base_render(bgl))
//            return false;
    } else {
        fprintf(stderr, "Invalid render API: 0x%04X\n", rndr_cfg->api);
        return false;
    }

    // if window->monitor ...
    // else
    {
        if (w_cfg->visible)
            show_platform_window(bgl);
    }

    return true;
}

void destroy_platform_window(bgl_instance bgl) {
    // if window->monitor
    //     release_monitor;

    if (bgl->window->platform.window) {
        RemovePropW(bgl->window->platform.window, L"BGL");
        DestroyWindow(bgl->window->platform.window);
        bgl->window->platform.window = NULL;
    }
}

void show_platform_window(bgl_instance bgl) {
    ShowWindow(bgl->window->platform.window, SW_SHOWNA);
}

void set_platform_window_title(bgl_instance bgl, const char *title) {
    WCHAR *w_title = bgl_create_wstr_from_utf8(title);
    if (!w_title)
        return;

    SetWindowTextW(bgl->window->platform.window, w_title);
    free(w_title);
}

int is_visible_platform_window(bgl_instance bgl) {
    return IsWindowVisible(bgl->window->platform.window);
}


/// window events

void poll_platform_window_events(bgl_instance bgl) {
    MSG msg;

    while (PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE)) {
        if (msg.message == WM_QUIT)
            input_window_close_request(bgl);
        else {
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }
    }

    HWND hw = GetActiveWindow();
    if (hw && bgl->window) {
        static const int keys[4][2] = {
                {VK_LSHIFT, BGL_KEY_LEFT_SHIFT},
                {VK_RSHIFT, BGL_KEY_RIGHT_SHIFT},
                {VK_LWIN, BGL_KEY_LEFT_SUPER},
                {VK_RWIN, BGL_KEY_RIGHT_SUPER},
        };

        for (int i = 0; i < 4; ++i) {
            bgl_key key = keys[i][1];

            if ((GetKeyState(keys[i][0]) & 0x8000)
                    || (bgl->window->keys[key].action != BGL_PRESS))
                continue;

            input_key(bgl, key, bgl->platform.scancodes[key], BGL_PRESS, get_key_mods());
        }
    }
}

void wait_platform_window_events(bgl_instance bgl) {
    WaitMessage();
    poll_platform_window_events(bgl);
}

void wait_platform_window_events_timeout(bgl_instance bgl, double t) {
    MsgWaitForMultipleObjects(0, NULL, FALSE, (DWORD)(t * 1e3), QS_ALLEVENTS);
    poll_platform_window_events(bgl);
}

void send_platform_window_empty_event(bgl_instance bgl) {
    PostMessageW(bgl->window->platform.window, WM_NULL, 0, 0);
}
