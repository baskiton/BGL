/*
 * Copyright (c) 2022 Alexander Baskikh
 *
 * MIT License (MIT), http://opensource.org/licenses/MIT
 * Full license can be found in the LICENSE file
 */

#include <errno.h>
#include <string.h>

#include <bgl/bglt.h>

#include "internal.h"


typedef struct {
    void *buf;
    int buf_sz;
    int cnt;
} obuf_t;

#define BUFFER_INIT { .buf_sz = 512 }

#define BUF_PUSH_BACK(bb, type, data)                                               \
    ({                                                                              \
        if ((bb.cnt >= bb.buf_sz || !bb.buf)                                        \
                && !(bb.buf = bgl_realloc_array(bb.buf, bb.buf_sz <<= 1, sizeof(type))))    \
            NULL;                                                                   \
        memcpy(&((type *)bb.buf)[bb.cnt++], &data, sizeof(type));                   \
    })

#define GROUP_INIT      \
        {               \
            if (!BUF_PUSH_BACK(groups, bgl_obj_group_t, (bgl_obj_group_t){0})) {    \
                err = errno;    \
                goto end;       \
            }                   \
            cur_grp = &((bgl_obj_group_t*)groups.buf)[groups.cnt - 1];  \
        }

static int parse_float(char *s, float v[6]) {
    int cnt = 0;
    while ((s = strchr(s, ' ')))
        v[cnt++] = strtof(++s, &s);
    return cnt;
}

typedef union {
    int v[3];
    struct {
        int vi, vti, vni;
    };
} oface_t;

static int parse_face(char *s, oface_t face[3]) {
    char tmp_buf[512];
    int cnt = 0;
    int i;
    memset(face, 0, sizeof(oface_t) * 3);
    s = strchr(s, ' ');
    while (s) {
        i = 0;
        face[cnt].v[i++] = (int)strtoull(++s, &s, 10);

        char *t = strchr(s, ' ');
        ssize_t n = t ? t - s : (ssize_t)sizeof(tmp_buf) - 1;
        strncpy(tmp_buf, s, n);
        tmp_buf[n] = '\0';
        s = t;
        t = tmp_buf;
        while (i < 3 && (t = strchr(t, '/'))) {
            int x = (int)strtoull(++t, &t, 10);
            if (x)
                face[cnt].v[i] = x;
            ++i;
        }
        ++cnt;
    }
    return cnt;
}

BGL_API bgl_obj_t *bgl_load_obj(const char *path) {
    FILE *obj_f = NULL;
    char *buf = NULL;
    int buf_sz = 512;
    int err = 0;

    if (!(obj_f = fopen(path, "r")))
        return NULL;

    if (!(buf = malloc(buf_sz))) {
        err = errno;
        goto end;
    }

    obuf_t groups = BUFFER_INIT;
    obuf_t v = BUFFER_INIT;
    obuf_t vn = BUFFER_INIT;
    obuf_t f = BUFFER_INIT;
    bgl_obj_group_t *cur_grp = NULL;

    float vval[6];
    oface_t fval[3];
    vertex vtx;
    vec3 normal;

    errno = 0;
    ssize_t n;
    while ((n = bgl_getline(&buf, &buf_sz, obj_f)) >= 0) {
        switch (*buf) {
        case 'o':   // object name
            if (groups.cnt) {
                if (f.cnt) {
                    cur_grp->i_cnt = f.cnt;
                    cur_grp->indices = bgl_realloc_array(f.buf, f.cnt, sizeof(*cur_grp->indices));
                    f.buf = NULL;
                    f.cnt = 0;
                }
            }
            GROUP_INIT
            cur_grp->name = bgl_strndup(&buf[2], n - 3);
            break;
        case 'v':   // vertex data
            switch (buf[1]) {
            case ' ':   // geometric vertices
                vtx = (vertex){.pos = GLM_VEC4_BLACK_INIT, .color = GLM_VEC4_ONE_INIT};

                switch (parse_float(&buf[1], vval)) {
                case 4:
                    vtx.pos[3] = vval[3];
                case 3:
                    glm_vec3_copy(vval, vtx.pos);
                    BUF_PUSH_BACK(v, vertex, vtx);
                    break;
                case 6:
                    glm_vec3_copy(vval, vtx.pos);
                    glm_vec3_copy(&vval[3], vtx.color);
                    BUF_PUSH_BACK(v, vertex, vtx);
                default:
                    break;
                }
                break;

            case 'n':   // vertex normal
                if (parse_float(&buf[1], vval) == 3) {
                    glm_vec3_copy(vval, normal);
                    BUF_PUSH_BACK(vn, vec3, normal);
                }
                break;

            }
            break;
        case 'f':   // face
            if (!groups.cnt)
                GROUP_INIT
            if (parse_face(buf, fval) == 3) {
                vindex idx = {0};
                for (int i = 0; i < 3; ++i) {
                    idx.idx = fval[i].vi - 1;
                    glm_vec4_copy(GLM_VEC4_ONE, idx.color);
                    if (fval[i].vni)
                        glm_vec3_copy(((vec3 *)vn.buf)[fval[i].vni - 1], idx.normal);
//                    else
//                        glm_vec3_copy((vec3){NAN, NAN, NAN}, idx.normal);
                    BUF_PUSH_BACK(f, vindex, idx);
                }
            }
            break;
        }
    }
    if (n == -1 && errno) {
        err = errno;
        goto end;
    }

    if (groups.cnt) {
        if (f.cnt) {
            cur_grp->i_cnt = f.cnt;
            cur_grp->indices = bgl_realloc_array(f.buf, f.cnt, sizeof(*cur_grp->indices));
            f.buf = NULL;
            f.cnt = 0;
        }
    }

end:
    free(buf);
    fclose(obj_f);

    bgl_obj_t *result;

    if (!err) {
        if (!((result = calloc(1, sizeof(*result)))
                && (result->groups = calloc(groups.cnt, sizeof(*result->groups)))))
            err = errno;
        else {
            result->v_cnt = v.cnt;
            result->vertices = bgl_realloc_array(v.buf, v.cnt, sizeof(*result->vertices));
            v.buf = NULL;

            result->group_cnt = groups.cnt;
            bgl_obj_group_t *grp = result->groups;
            for (size_t i = result->group_cnt; i--; ++grp) {
                bgl_obj_group_t *bb = &((bgl_obj_group_t *)groups.buf)[i];
                grp->name = bb->name;
                if ((grp->i_cnt = bb->i_cnt))
                    grp->indices = bb->indices;
            }
        }
    }

    if (err) {
        for (bgl_obj_group_t *grp = groups.buf; groups.cnt--; ++grp) {
            free(grp->name);
            free(grp->indices);
        }
    }

    free(groups.buf);
    free(v.buf);
    free(vn.buf);
    free(f.buf);
    errno = err;

    return result;
}

BGL_API void bgl_destroy_obj(bgl_obj_t *obj) {
    free(obj->vertices);
    while (obj->group_cnt--) {
        free(obj->groups[obj->group_cnt].name);
        free(obj->groups[obj->group_cnt].indices);
    }
    free(obj->groups);
    free(obj);
}
