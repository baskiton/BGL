/*
 * Copyright (c) 2022 Alexander Baskikh
 *
 * MIT License (MIT), http://opensource.org/licenses/MIT
 * Full license can be found in the LICENSE file
 */

#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "internal.h"


static short translate_keysyms(const KeySym *ks, int width) {
    if (width > 1) {
        switch (ks[1]) {
        case XK_KP_0:
            return BGL_KEY_KP_0;
        case XK_KP_1:
            return BGL_KEY_KP_1;
        case XK_KP_2:
            return BGL_KEY_KP_2;
        case XK_KP_3:
            return BGL_KEY_KP_3;
        case XK_KP_4:
            return BGL_KEY_KP_4;
        case XK_KP_5:
            return BGL_KEY_KP_5;
        case XK_KP_6:
            return BGL_KEY_KP_6;
        case XK_KP_7:
            return BGL_KEY_KP_7;
        case XK_KP_8:
            return BGL_KEY_KP_8;
        case XK_KP_9:
            return BGL_KEY_KP_9;
        case XK_KP_Separator:
        case XK_KP_Decimal:
            return BGL_KEY_KP_DECIMAL;
        case XK_KP_Equal:
            return BGL_KEY_KP_EQUAL;
        case XK_KP_Enter:
            return BGL_KEY_KP_ENTER;
        default:
            break;
        }
    }

    switch (ks[0]) {
    case XK_Escape:
        return BGL_KEY_ESCAPE;
    case XK_Tab:
        return BGL_KEY_TAB;
    case XK_Shift_L:
        return BGL_KEY_LEFT_SHIFT;
    case XK_Shift_R:
        return BGL_KEY_RIGHT_SHIFT;
    case XK_Control_L:
        return BGL_KEY_LEFT_CONTROL;
    case XK_Control_R:
        return BGL_KEY_RIGHT_CONTROL;
    case XK_Meta_L:
    case XK_Alt_L:
        return BGL_KEY_LEFT_ALT;
    case XK_Mode_switch:        // Mapped to Alt_R on many keyboards
    case XK_ISO_Level3_Shift:   // AltGr on at least some machines
    case XK_Meta_R:
    case XK_Alt_R:
        return BGL_KEY_RIGHT_ALT;
    case XK_Super_L:
        return BGL_KEY_LEFT_SUPER;
    case XK_Super_R:
        return BGL_KEY_RIGHT_SUPER;
    case XK_Menu:
        return BGL_KEY_MENU;
    case XK_Num_Lock:
        return BGL_KEY_NUM_LOCK;
    case XK_Caps_Lock:
        return BGL_KEY_CAPS_LOCK;
    case XK_Print:
        return BGL_KEY_PRINT_SCREEN;
    case XK_Scroll_Lock:
        return BGL_KEY_SCROLL_LOCK;
    case XK_Pause:
        return BGL_KEY_PAUSE;
    case XK_Delete:
        return BGL_KEY_DELETE;
    case XK_BackSpace:
        return BGL_KEY_BACKSPACE;
    case XK_Return:
        return BGL_KEY_ENTER;
    case XK_Home:
        return BGL_KEY_HOME;
    case XK_End:
        return BGL_KEY_END;
    case XK_Page_Up:
        return BGL_KEY_PAGE_UP;
    case XK_Page_Down:
        return BGL_KEY_PAGE_DOWN;
    case XK_Insert:
        return BGL_KEY_INSERT;
    case XK_Left:
        return BGL_KEY_LEFT;
    case XK_Right:
        return BGL_KEY_RIGHT;
    case XK_Down:
        return BGL_KEY_DOWN;
    case XK_Up:
        return BGL_KEY_UP;
    case XK_F1:
        return BGL_KEY_F1;
    case XK_F2:
        return BGL_KEY_F2;
    case XK_F3:
        return BGL_KEY_F3;
    case XK_F4:
        return BGL_KEY_F4;
    case XK_F5:
        return BGL_KEY_F5;
    case XK_F6:
        return BGL_KEY_F6;
    case XK_F7:
        return BGL_KEY_F7;
    case XK_F8:
        return BGL_KEY_F8;
    case XK_F9:
        return BGL_KEY_F9;
    case XK_F10:
        return BGL_KEY_F10;
    case XK_F11:
        return BGL_KEY_F11;
    case XK_F12:
        return BGL_KEY_F12;
    case XK_F13:
        return BGL_KEY_F13;
    case XK_F14:
        return BGL_KEY_F14;
    case XK_F15:
        return BGL_KEY_F15;
    case XK_F16:
        return BGL_KEY_F16;
    case XK_F17:
        return BGL_KEY_F17;
    case XK_F18:
        return BGL_KEY_F18;
    case XK_F19:
        return BGL_KEY_F19;
    case XK_F20:
        return BGL_KEY_F20;
    case XK_F21:
        return BGL_KEY_F21;
    case XK_F22:
        return BGL_KEY_F22;
    case XK_F23:
        return BGL_KEY_F23;
    case XK_F24:
        return BGL_KEY_F24;
    case XK_F25:
        return BGL_KEY_F25;

    // Numeric keypad
    case XK_KP_Divide:
        return BGL_KEY_KP_DIVIDE;
    case XK_KP_Multiply:
        return BGL_KEY_KP_MULTIPLY;
    case XK_KP_Subtract:
        return BGL_KEY_KP_SUBTRACT;
    case XK_KP_Add:
        return BGL_KEY_KP_ADD;

    // These should have been detected in secondary keysym test above!
    case XK_KP_Insert:
        return BGL_KEY_KP_0;
    case XK_KP_End:
        return BGL_KEY_KP_1;
    case XK_KP_Down:
        return BGL_KEY_KP_2;
    case XK_KP_Page_Down:
        return BGL_KEY_KP_3;
    case XK_KP_Left:
        return BGL_KEY_KP_4;
    case XK_KP_Right:
        return BGL_KEY_KP_6;
    case XK_KP_Home:
        return BGL_KEY_KP_7;
    case XK_KP_Up:
        return BGL_KEY_KP_8;
    case XK_KP_Page_Up:
        return BGL_KEY_KP_9;
    case XK_KP_Delete:
        return BGL_KEY_KP_DECIMAL;
    case XK_KP_Equal:
        return BGL_KEY_KP_EQUAL;
    case XK_KP_Enter:
        return BGL_KEY_KP_ENTER;

    // Last resort: Check for printable keys (should not happen if the XKB
    // extension is available). This will give a layout dependent mapping
    // (which is wrong, and we may miss some keys, especially on non-US
    // keyboards), but it's better than nothing...
    case XK_a:
        return BGL_KEY_A;
    case XK_b:
        return BGL_KEY_B;
    case XK_c:
        return BGL_KEY_C;
    case XK_d:
        return BGL_KEY_D;
    case XK_e:
        return BGL_KEY_E;
    case XK_f:
        return BGL_KEY_F;
    case XK_g:
        return BGL_KEY_G;
    case XK_h:
        return BGL_KEY_H;
    case XK_i:
        return BGL_KEY_I;
    case XK_j:
        return BGL_KEY_J;
    case XK_k:
        return BGL_KEY_K;
    case XK_l:
        return BGL_KEY_L;
    case XK_m:
        return BGL_KEY_M;
    case XK_n:
        return BGL_KEY_N;
    case XK_o:
        return BGL_KEY_O;
    case XK_p:
        return BGL_KEY_P;
    case XK_q:
        return BGL_KEY_Q;
    case XK_r:
        return BGL_KEY_R;
    case XK_s:
        return BGL_KEY_S;
    case XK_t:
        return BGL_KEY_T;
    case XK_u:
        return BGL_KEY_U;
    case XK_v:
        return BGL_KEY_V;
    case XK_w:
        return BGL_KEY_W;
    case XK_x:
        return BGL_KEY_X;
    case XK_y:
        return BGL_KEY_Y;
    case XK_z:
        return BGL_KEY_Z;
    case XK_1:
        return BGL_KEY_1;
    case XK_2:
        return BGL_KEY_2;
    case XK_3:
        return BGL_KEY_3;
    case XK_4:
        return BGL_KEY_4;
    case XK_5:
        return BGL_KEY_5;
    case XK_6:
        return BGL_KEY_6;
    case XK_7:
        return BGL_KEY_7;
    case XK_8:
        return BGL_KEY_8;
    case XK_9:
        return BGL_KEY_9;
    case XK_0:
        return BGL_KEY_0;
    case XK_space:
        return BGL_KEY_SPACE;
    case XK_minus:
        return BGL_KEY_MINUS;
    case XK_equal:
        return BGL_KEY_EQUAL;
    case XK_bracketleft:
        return BGL_KEY_LEFT_BRACKET;
    case XK_bracketright:
        return BGL_KEY_RIGHT_BRACKET;
    case XK_backslash:
        return BGL_KEY_BACKSLASH;
    case XK_semicolon:
        return BGL_KEY_SEMICOLON;
    case XK_apostrophe:
        return BGL_KEY_APOSTROPHE;
    case XK_grave:
        return BGL_KEY_GRAVE_ACCENT;
    case XK_comma:
        return BGL_KEY_COMMA;
    case XK_period:
        return BGL_KEY_PERIOD;
    case XK_slash:
        return BGL_KEY_SLASH;
    case XK_less:
        return BGL_KEY_WORLD_1; // At least in some layouts...
    default:
        break;
    }

    return BGL_KEY_UNKNOWN;
}

static void create_key_tables(bgl_instance bgl) {
    memset(bgl->platform.keycodes, -1, sizeof(bgl->platform.keycodes));

    int sc_min, sc_max, width;

    // TODO: use XKb
    XDisplayKeycodes(bgl->platform.display, &sc_min, &sc_max);

    KeySym *keysyms = XGetKeyboardMapping(bgl->platform.display, sc_min, sc_max - sc_min + 1, &width);

    for (int sc = sc_min; sc <= sc_max; ++sc)
        if (bgl->platform.keycodes[sc] < 0)
            bgl->platform.keycodes[sc] = translate_keysyms(&keysyms[(sc - sc_min) * width], width);

    XFree(keysyms);
}

int init_platform(bgl_instance bgl) {
//    XInitThreads();
//    XrmInitialize();

    if (!(bgl->platform.display = XOpenDisplay(NULL))) {
        fprintf(stderr, "X11: Failed to open display: %s\n",
                getenv("DISPLAY") ?: "`DISPLAY` environment is missing");
        return false;
    }

    bgl->platform.screen = XDefaultScreen(bgl->platform.display);
    bgl->platform.root = XRootWindow(bgl->platform.display, bgl->platform.screen);
    bgl->platform.context = XUniqueContext();

    if (pipe(bgl->platform.empty_evt_pipe)) {
        fprintf(stderr, "X11: Failed to create empty event pipe: %s\n", strerror(errno));
        return false;
    }
    for (int i = 0; i < 2; ++i) {
        const int fl = fcntl(bgl->platform.empty_evt_pipe[1], F_GETFL, 0);
        const int fd = fcntl(bgl->platform.empty_evt_pipe[1], F_GETFD, 0);
        if (fl == -1 || fd == -1
            || fcntl(bgl->platform.empty_evt_pipe[i], F_SETFL, fl | O_NONBLOCK) == -1
            || fcntl(bgl->platform.empty_evt_pipe[i], F_SETFL, fd | FD_CLOEXEC) == -1) {
            fprintf(stderr, "X11: Failed to set flags for empty event pipe: %s\n", strerror(errno));
            return false;
        }
    }

    // extensions
    XInternAtoms(bgl->platform.display,
                 (char **)atom_names,
                 sizeof(bgl->platform.atoms) / sizeof(Atom),
                 False,
                 (Atom *)&bgl->platform.atoms);

    // TODO: Create key code translation tables
    create_key_tables(bgl);

    return true;
}

void terminate_platform(bgl_instance bgl) {
    bgl_platform x11 = bgl->platform;

    if (x11.display) {
        XCloseDisplay(x11.display);
        x11.display = NULL;
    }

    // TODO:

    if (x11.empty_evt_pipe[0] || x11.empty_evt_pipe[1]) {
        close(x11.empty_evt_pipe[0]);
        close(x11.empty_evt_pipe[1]);
    }
}
