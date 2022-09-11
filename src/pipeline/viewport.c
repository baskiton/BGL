/*
 * Copyright (c) 2022 Alexander Baskikh
 *
 * MIT License (MIT), http://opensource.org/licenses/MIT
 * Full license can be found in the LICENSE file
 */

#include "internal.h"



///////////////////////////////////////////////////////////////////////////////

BGL_API void bgl_set_viewport(bgl_instance bgl, bgl_viewport *vp) {
    if ((bgl->viewport.x = vp->x) < 0)
        ++bgl->viewport.x;
    else if (bgl->viewport.x > 0)
        --bgl->viewport.x;

    if ((bgl->viewport.y = vp->y) < 0)
        ++bgl->viewport.y;
    else if (bgl->viewport.y > 0)
        --bgl->viewport.y;

    if ((bgl->viewport.px = vp->width) < 0)
        ++bgl->viewport.px;
    else if (bgl->viewport.px > 0)
        --bgl->viewport.px;

    if ((bgl->viewport.py = vp->height) < 0)
        ++bgl->viewport.py;
    else if (bgl->viewport.py > 0)
        --bgl->viewport.py;

    bgl->viewport.pz = 1.0f - 0.0f; // or (1.0f - 0.0f) / 2
    bgl->viewport.pxh = bgl->viewport.px / 2.0f;
    bgl->viewport.pyh = bgl->viewport.py / 2.0f;
    bgl->viewport.ox = bgl->viewport.x + bgl->viewport.pxh;
    bgl->viewport.oy = bgl->viewport.y + bgl->viewport.pyh;
    bgl->viewport.oz = 0.0f;    // or (1.0 + 0.0) / 2
    bgl->viewport.aspect_ratio = bgl->viewport.px / (bgl->viewport.py < 0 ? -bgl->viewport.py : bgl->viewport.py);
}

BGL_API float bgl_get_viewport_aspect_ratio(bgl_instance bgl) {
    return bgl->viewport.aspect_ratio;
}
