
/*
 * Copyright (c) 2022 Alexander Baskikh
 *
 * MIT License (MIT), http://opensource.org/licenses/MIT
 * Full license can be found in the LICENSE file
 */

#ifndef BGL_BGLT_H
#define BGL_BGLT_H


#include <bgl/bgl.h>


typedef struct {
    char *name;
    vindex *indices;
    int i_cnt;
} bgl_obj_group_t;

typedef struct {
    vertex *vertices;
    int v_cnt;
    bgl_obj_group_t *groups;
    int group_cnt;
} bgl_obj_t;

BGL_API bgl_obj_t *bgl_load_obj(const char *path);
BGL_API void bgl_destroy_obj(bgl_obj_t *obj);

#endif // BGL_BGLT_H
