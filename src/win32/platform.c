/*
 * Copyright (c) 2022 Alexander Baskikh
 *
 * MIT License (MIT), http://opensource.org/licenses/MIT
 * Full license can be found in the LICENSE file
 */

#include "internal.h"


WCHAR *bgl_create_wstr_from_utf8(const char *s) {
    int cnt = MultiByteToWideChar(CP_UTF8, 0, s, -1, NULL, 0);
    if (!cnt) {
        fprintf(stderr, "Win32: Failed to convert string from utf-8 (1): s=%p\n", s);
        return NULL;
    }

    WCHAR *res = calloc(cnt, sizeof(WCHAR));

    if (!MultiByteToWideChar(CP_UTF8, 0, s, -1, res, cnt)) {
        free(res);
        fprintf(stderr, "Win32: Failed to convert string from utf-8 (2)\n");
        return NULL;
    }

    return res;
}