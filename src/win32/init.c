/*
 * Copyright (c) 2022 Alexander Baskikh
 *
 * MIT License (MIT), http://opensource.org/licenses/MIT
 * Full license can be found in the LICENSE file
 */

#include <stdbool.h>

#include "internal.h"

#include "platform.h"


static void create_key_tables(bgl_instance bgl) {
    memset(bgl->platform.keycodes, -1, sizeof(bgl->platform.keycodes));
    memset(bgl->platform.scancodes, -1, sizeof(bgl->platform.scancodes));

    bgl->platform.keycodes[0x00B] = BGL_KEY_0;
    bgl->platform.keycodes[0x002] = BGL_KEY_1;
    bgl->platform.keycodes[0x003] = BGL_KEY_2;
    bgl->platform.keycodes[0x004] = BGL_KEY_3;
    bgl->platform.keycodes[0x005] = BGL_KEY_4;
    bgl->platform.keycodes[0x006] = BGL_KEY_5;
    bgl->platform.keycodes[0x007] = BGL_KEY_6;
    bgl->platform.keycodes[0x008] = BGL_KEY_7;
    bgl->platform.keycodes[0x009] = BGL_KEY_8;
    bgl->platform.keycodes[0x00A] = BGL_KEY_9;
    bgl->platform.keycodes[0x01E] = BGL_KEY_A;
    bgl->platform.keycodes[0x030] = BGL_KEY_B;
    bgl->platform.keycodes[0x02E] = BGL_KEY_C;
    bgl->platform.keycodes[0x020] = BGL_KEY_D;
    bgl->platform.keycodes[0x012] = BGL_KEY_E;
    bgl->platform.keycodes[0x021] = BGL_KEY_F;
    bgl->platform.keycodes[0x022] = BGL_KEY_G;
    bgl->platform.keycodes[0x023] = BGL_KEY_H;
    bgl->platform.keycodes[0x017] = BGL_KEY_I;
    bgl->platform.keycodes[0x024] = BGL_KEY_J;
    bgl->platform.keycodes[0x025] = BGL_KEY_K;
    bgl->platform.keycodes[0x026] = BGL_KEY_L;
    bgl->platform.keycodes[0x032] = BGL_KEY_M;
    bgl->platform.keycodes[0x031] = BGL_KEY_N;
    bgl->platform.keycodes[0x018] = BGL_KEY_O;
    bgl->platform.keycodes[0x019] = BGL_KEY_P;
    bgl->platform.keycodes[0x010] = BGL_KEY_Q;
    bgl->platform.keycodes[0x013] = BGL_KEY_R;
    bgl->platform.keycodes[0x01F] = BGL_KEY_S;
    bgl->platform.keycodes[0x014] = BGL_KEY_T;
    bgl->platform.keycodes[0x016] = BGL_KEY_U;
    bgl->platform.keycodes[0x02F] = BGL_KEY_V;
    bgl->platform.keycodes[0x011] = BGL_KEY_W;
    bgl->platform.keycodes[0x02D] = BGL_KEY_X;
    bgl->platform.keycodes[0x015] = BGL_KEY_Y;
    bgl->platform.keycodes[0x02C] = BGL_KEY_Z;

    bgl->platform.keycodes[0x028] = BGL_KEY_APOSTROPHE;
    bgl->platform.keycodes[0x02B] = BGL_KEY_BACKSLASH;
    bgl->platform.keycodes[0x033] = BGL_KEY_COMMA;
    bgl->platform.keycodes[0x00D] = BGL_KEY_EQUAL;
    bgl->platform.keycodes[0x029] = BGL_KEY_GRAVE_ACCENT;
    bgl->platform.keycodes[0x01A] = BGL_KEY_LEFT_BRACKET;
    bgl->platform.keycodes[0x00C] = BGL_KEY_MINUS;
    bgl->platform.keycodes[0x034] = BGL_KEY_PERIOD;
    bgl->platform.keycodes[0x01B] = BGL_KEY_RIGHT_BRACKET;
    bgl->platform.keycodes[0x027] = BGL_KEY_SEMICOLON;
    bgl->platform.keycodes[0x035] = BGL_KEY_SLASH;
    bgl->platform.keycodes[0x056] = BGL_KEY_WORLD_2;

    bgl->platform.keycodes[0x00E] = BGL_KEY_BACKSPACE;
    bgl->platform.keycodes[0x153] = BGL_KEY_DELETE;
    bgl->platform.keycodes[0x14F] = BGL_KEY_END;
    bgl->platform.keycodes[0x01C] = BGL_KEY_ENTER;
    bgl->platform.keycodes[0x001] = BGL_KEY_ESCAPE;
    bgl->platform.keycodes[0x147] = BGL_KEY_HOME;
    bgl->platform.keycodes[0x152] = BGL_KEY_INSERT;
    bgl->platform.keycodes[0x15D] = BGL_KEY_MENU;
    bgl->platform.keycodes[0x151] = BGL_KEY_PAGE_DOWN;
    bgl->platform.keycodes[0x149] = BGL_KEY_PAGE_UP;
    bgl->platform.keycodes[0x045] = BGL_KEY_PAUSE;
    bgl->platform.keycodes[0x039] = BGL_KEY_SPACE;
    bgl->platform.keycodes[0x00F] = BGL_KEY_TAB;
    bgl->platform.keycodes[0x03A] = BGL_KEY_CAPS_LOCK;
    bgl->platform.keycodes[0x145] = BGL_KEY_NUM_LOCK;
    bgl->platform.keycodes[0x046] = BGL_KEY_SCROLL_LOCK;
    bgl->platform.keycodes[0x03B] = BGL_KEY_F1;
    bgl->platform.keycodes[0x03C] = BGL_KEY_F2;
    bgl->platform.keycodes[0x03D] = BGL_KEY_F3;
    bgl->platform.keycodes[0x03E] = BGL_KEY_F4;
    bgl->platform.keycodes[0x03F] = BGL_KEY_F5;
    bgl->platform.keycodes[0x040] = BGL_KEY_F6;
    bgl->platform.keycodes[0x041] = BGL_KEY_F7;
    bgl->platform.keycodes[0x042] = BGL_KEY_F8;
    bgl->platform.keycodes[0x043] = BGL_KEY_F9;
    bgl->platform.keycodes[0x044] = BGL_KEY_F10;
    bgl->platform.keycodes[0x057] = BGL_KEY_F11;
    bgl->platform.keycodes[0x058] = BGL_KEY_F12;
    bgl->platform.keycodes[0x064] = BGL_KEY_F13;
    bgl->platform.keycodes[0x065] = BGL_KEY_F14;
    bgl->platform.keycodes[0x066] = BGL_KEY_F15;
    bgl->platform.keycodes[0x067] = BGL_KEY_F16;
    bgl->platform.keycodes[0x068] = BGL_KEY_F17;
    bgl->platform.keycodes[0x069] = BGL_KEY_F18;
    bgl->platform.keycodes[0x06A] = BGL_KEY_F19;
    bgl->platform.keycodes[0x06B] = BGL_KEY_F20;
    bgl->platform.keycodes[0x06C] = BGL_KEY_F21;
    bgl->platform.keycodes[0x06D] = BGL_KEY_F22;
    bgl->platform.keycodes[0x06E] = BGL_KEY_F23;
    bgl->platform.keycodes[0x076] = BGL_KEY_F24;
    bgl->platform.keycodes[0x038] = BGL_KEY_LEFT_ALT;
    bgl->platform.keycodes[0x01D] = BGL_KEY_LEFT_CONTROL;
    bgl->platform.keycodes[0x02A] = BGL_KEY_LEFT_SHIFT;
    bgl->platform.keycodes[0x15B] = BGL_KEY_LEFT_SUPER;
    bgl->platform.keycodes[0x137] = BGL_KEY_PRINT_SCREEN;
    bgl->platform.keycodes[0x138] = BGL_KEY_RIGHT_ALT;
    bgl->platform.keycodes[0x11D] = BGL_KEY_RIGHT_CONTROL;
    bgl->platform.keycodes[0x036] = BGL_KEY_RIGHT_SHIFT;
    bgl->platform.keycodes[0x15C] = BGL_KEY_RIGHT_SUPER;
    bgl->platform.keycodes[0x150] = BGL_KEY_DOWN;
    bgl->platform.keycodes[0x14B] = BGL_KEY_LEFT;
    bgl->platform.keycodes[0x14D] = BGL_KEY_RIGHT;
    bgl->platform.keycodes[0x148] = BGL_KEY_UP;

    bgl->platform.keycodes[0x052] = BGL_KEY_KP_0;
    bgl->platform.keycodes[0x04F] = BGL_KEY_KP_1;
    bgl->platform.keycodes[0x050] = BGL_KEY_KP_2;
    bgl->platform.keycodes[0x051] = BGL_KEY_KP_3;
    bgl->platform.keycodes[0x04B] = BGL_KEY_KP_4;
    bgl->platform.keycodes[0x04C] = BGL_KEY_KP_5;
    bgl->platform.keycodes[0x04D] = BGL_KEY_KP_6;
    bgl->platform.keycodes[0x047] = BGL_KEY_KP_7;
    bgl->platform.keycodes[0x048] = BGL_KEY_KP_8;
    bgl->platform.keycodes[0x049] = BGL_KEY_KP_9;
    bgl->platform.keycodes[0x04E] = BGL_KEY_KP_ADD;
    bgl->platform.keycodes[0x053] = BGL_KEY_KP_DECIMAL;
    bgl->platform.keycodes[0x135] = BGL_KEY_KP_DIVIDE;
    bgl->platform.keycodes[0x11C] = BGL_KEY_KP_ENTER;
    bgl->platform.keycodes[0x059] = BGL_KEY_KP_EQUAL;
    bgl->platform.keycodes[0x037] = BGL_KEY_KP_MULTIPLY;
    bgl->platform.keycodes[0x04A] = BGL_KEY_KP_SUBTRACT;

    for (unsigned sc = 0; sc < 512; ++sc)
        if (bgl->platform.keycodes[sc] > 0)
            bgl->platform.scancodes[bgl->platform.keycodes[sc]] = sc;
}

int init_platform(bgl_instance bgl) {
    /*
    // user32.dll
    SetProcessDPIAware();
    ChangeWindowMessageFilterEx();
    EnableNonClientDpiScaling();
    SetProcessDpiAwarenessContext();
    GetDpiForWindow();
    AdjustWindowRectExForDpi();
    GetSystemMetricsForDpi();

    // dinput8.dll
    DirectInput8Create();

    // xinput*.dll
    XInputGetCapabilities();
    XInputGetState();

    // dwmapi.dll
    DwmIsCompositionEnabled();
    DwmFlush();
    DwmEnableBlurBehindWindow();
    DwmGetColorizationColor();

    // shcore.dll
    SetProcessDpiAwareness();
    GetDpiForMonitor();

    // ntdll.dll
    RtlVerifyVersionInfo();
     // */

    create_key_tables(bgl);

    // TODO: SetProcessDpi
    // TODO: create helper window?
    // TODO: poll monitor

    return true;
}

void terminate_platform(bgl_instance bgl) {
    if (bgl->platform.main_window_class)
        UnregisterClassW(MAKEINTATOM(bgl->platform.main_window_class), bgl->platform.instance);
}
