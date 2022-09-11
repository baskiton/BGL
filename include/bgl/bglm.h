/*
 * Copyright (c) 2022 Alexander Baskikh
 *
 * MIT License (MIT), http://opensource.org/licenses/MIT
 * Full license can be found in the LICENSE file
 */

#ifndef BGL_BGLM_H
#define BGL_BGLM_H

#include <cglm/cglm.h>
#include <cglm/struct.h>
#include <cglm/types-struct.h>


typedef struct {
    vec4 pos;
    vec4 color;
    vec3 normal;
} vertex;

typedef struct {
    int idx;
    vec4 color;
    vec3 normal;
} vindex;

typedef struct {
    mat4 model;
    mat4 view;
    mat4 proj;
} uniform;

typedef struct {
    mat4 *model;
    mat4 *view;
    mat4 *proj;
} uniform_p;

#define BGL_UNIFORM_INIT {GLM_MAT4_IDENTITY_INIT, GLM_MAT4_IDENTITY_INIT, GLM_MAT4_IDENTITY_INIT}


#endif // BGL_BGLM_H
