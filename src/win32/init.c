/*
 * Copyright (c) 2022 Alexander Baskikh
 *
 * MIT License (MIT), http://opensource.org/licenses/MIT
 * Full license can be found in the LICENSE file
 */

#include <stdbool.h>

#include "internal.h"

#include "platform.h"
#include "win32/platform.h"


int init_platform(bgl_instance bgl) {
    return false;
}

void terminate_platform(bgl_instance bgl) {
}
