/*
 * Copyright (c) 2022 Alexander Baskikh
 *
 * MIT License (MIT), http://opensource.org/licenses/MIT
 * Full license can be found in the LICENSE file
 */

#ifndef _GNU_SOURCE
# define _GNU_SOURCE
#endif

#include "internal.h"


#define VHB_INIT(bgl, name) helper_buf *name = &bgl->dev.hb1
#define IHB_INIT(bgl, name) helper_buf *name = &bgl->dev.hb2
#define CHB_INIT(bgl, name) helper_buf *name = &bgl->dev.hb3

void loc_to_dev(mat4 vp, vec4 src, vec3 dst) {
    vec4 t;
    glm_mat4_mulv(vp, src, t);
    glm_vec3_divs(t, t[3] < FLT_MIN ? FLT_MIN : t[3], dst);
}

void dev_to_fb(bgl_viewport_internal *viewport, vec3 src, vec3 dst) {
    dst[0] = viewport->pxh * src[0] + viewport->ox;
    dst[1] = viewport->pyh * src[1] + viewport->oy;
    dst[2] = viewport->pz * src[2] + viewport->oz;
}

void dev_to_fbi(bgl_viewport_internal *viewport, vec3 src, ivec3 dst) {
    dst[0] = (int)(viewport->pxh * src[0] + viewport->ox);
    dst[1] = (int)(viewport->pyh * src[1] + viewport->oy);
    dst[2] = (int)(viewport->pz * src[2] + viewport->oz);
}

void loc_to_fb(mat4 vp, bgl_viewport_internal *viewport, vec4 src, vec3 dst) {
    vec4 t;
    glm_mat4_mulv(vp, src, t);
    glm_vec3_divs(t, t[3], t);

    dst[0] = viewport->pxh * t[0] + viewport->ox;
    dst[1] = viewport->pyh * t[1] + viewport->oy;
    dst[2] = viewport->pz * t[2] + viewport->oz;
}

void triangle_normal(vec3 a, vec3 b, vec3 c, vec3 dst) {
    vec3 u, v;
    glm_vec3_sub(b, a, u);
    glm_vec3_sub(c, a, v);
    glm_vec3_crossn(u, v, dst);
}

idx_item *push_back_helper_buf_idx(bgl_instance bgl, int v_off, ivec3 idxs, vec4 color, int n) {
    IHB_INIT(bgl, ihb);
    VHB_INIT(bgl, vhb);

    if ((ihb->cnt >= ihb->buf_sz || !ihb->buf)
            && !(ihb->buf = realloc(ihb->buf, (ihb->buf_sz <<= 1) * sizeof(idx_item))))
        return NULL;

    idx_item *ibuf = (idx_item *)ihb->buf + ihb->cnt++;

    glm_ivec3_copy(idxs, ibuf->tri);
    glm_vec4_copy(color, ibuf->color);
    ibuf->n = n;
    ibuf->v_off = v_off;
    ibuf->avg_z = NAN;

    return ibuf;
}

static int push_back_helper_buf_vtx(bgl_instance bgl, vec4 v) {
    VHB_INIT(bgl, vhb);

    if ((vhb->cnt >= vhb->buf_sz || !vhb->buf)
            && !(vhb->buf = realloc(vhb->buf, (vhb->buf_sz <<= 1) * sizeof(vertex_item))))
        return -1;

    vertex_item *vbuf = (vertex_item *)vhb->buf + vhb->cnt;

    glm_vec4_copy(v, vbuf->vtx);
    vbuf->used = 0;

    return vhb->cnt++;
}

void clear_helper_buf(bgl_instance bgl) {
    free(bgl->dev.hb1.buf);
    free(bgl->dev.hb2.buf);
    free(bgl->dev.hb3.buf);
    bgl->dev.hb1.buf_sz = bgl->dev.hb1.cnt
            = bgl->dev.hb2.buf_sz = bgl->dev.hb2.cnt
                    = bgl->dev.hb3.buf_sz = bgl->dev.hb3.cnt = 0;
}

void prepare_buffers(bgl_instance bgl, vec4 camera, vec4 light, mat4 vp, vertex_item **vhb_buf) {
    VHB_INIT(bgl, vhb);
    mat4 *model;
    mat3 mt;
    vec3 cam_dir;
    int new_sz;

    if (bgl->glob_uniform.view) {
        glm_mat4_copy(*bgl->glob_uniform.view, vp);

        glm_mat4_pick3t(vp, mt);
        glm_mat3_mulv(mt, vp[3], camera);
        glm_vec3_inv(camera);

        cam_dir[0] = vp[0][2];
        cam_dir[1] = vp[1][2];
        cam_dir[2] = vp[2][2];
    } else {
        glm_mat4_identity(vp);
        glm_vec4_copy((vec4) {0, 0, -1, 1}, camera);
        glm_vec3_copy((vec3) {0, 0, -1}, cam_dir);
    }

    if (bgl->glob_uniform.proj)
        glm_mat4_mul(*bgl->glob_uniform.proj, vp, vp);

    glm_vec3_normalize_to(cam_dir, light);
    light[3] = 0;

    for (bgl_vertex_buffer vbuf = bgl->vertex_buffer; vbuf; vbuf = vbuf->next) {
        model = vbuf->model_m ? : bgl->glob_uniform.model;
//        glm_mat4_inv(*model, nmodel);
//        glm_mat4_transpose(nmodel);

        if ((new_sz = vhb->cnt + vbuf->count) >= vhb->buf_sz || !vhb->buf) {
            int n = 0, sz = new_sz;
            for (; sz; sz >>= 1, ++n);
            sz = 1 << n;
            vhb->buf_sz = sz > vhb->buf_sz ? sz : vhb->buf_sz;

            if (!(vhb->buf = realloc(vhb->buf, vhb->buf_sz * sizeof(vertex_item))))
                return;
        }

        vertex_item *vitem = &((vertex_item *)vhb->buf)[vhb->cnt];
        vbuf->vitem_off = vhb->cnt;
        vhb->cnt = new_sz;

        if (*model)
            for (int j = 0; j < vbuf->count; ++j) {
                glm_mat4_mulv(*model, vbuf->vertices[j].pos, vitem[j].vtx);
                vitem[j].used = 0;
            }
        else
            for (int j = 0; j < vbuf->count; ++j) {
                glm_vec4_copy(vbuf->vertices[j].pos, vitem[j].vtx);
                vitem[j].used = 0;
            }
    }

    *vhb_buf = vhb->buf;
}

#ifdef __COMPAR_FN_T
typedef __compar_fn_t cmp_fn_t;
#else
typedef int (*cmp_fn_t)(const void *, const void *);
#endif

static int cmp_idx(const idx_item *a, const idx_item *b) {
    if (a->avg_z < b->avg_z)
        return 1;
    if (a->avg_z > b->avg_z)
        return -1;
    return 0;
}

static void vec_intersect_plane(clip_plane *plane, vec3 a, vec3 b, vec3 dst) {
    float ad = glm_vec3_dot(a, plane->norm);
    float bd = glm_vec3_dot(b, plane->norm);
    float t = (plane->d - ad) / (bd - ad);

    glm_vec3_sub(b, a, dst);
    glm_vec3_scale(dst, t, dst);
    glm_vec3_add(a, dst, dst);
}

static void clip_tri(bgl_instance bgl, idx_item *ibuf, ivec3 out[64], ivec3 **start, ivec3 **end) {
    static const clip_plane clip_planes[] = {
            {{0, 0, -1}, {0, 0, 1}, -1},    // near Z
            {{0, 0, 1}, {0, 0, -1}, -1},    // far Z
            {{0, 1, 0}, {0, -1, 0}, -1},    // top
            {{-1, 0, 0}, {1, 0, 0}, -1},    // left
            {{0, -1, 0}, {0, 1, 0}, -1},    // bottom
            {{1, 0, 0}, {-1, 0, 0}, -1},    // right
    };

    VHB_INIT(bgl, vhb);

    ivec3 *to_clipped_hd = out, *to_clipped_tail = out, *t;
    vec4 vt;

    float d0, d1, d2;
    int insides[3], outsides[3];
    int inside_cnt, outside_cnt;
    vertex_item *vertices = &((vertex_item *)vhb->buf)[ibuf->v_off];

    glm_ivec3_copy(ibuf->tri, *to_clipped_tail++);

    for (clip_plane *plane = (clip_plane *)clip_planes; plane < &clip_planes[sizeof(clip_planes) / sizeof(*clip_planes)]; ++plane) {
        for (t = to_clipped_tail; to_clipped_hd < t; ++to_clipped_hd) {
            inside_cnt = outside_cnt = 0;
            d0 = glm_vec3_dot(plane->norm, vertices[(*to_clipped_hd)[0]].vtx) - plane->d;
            d1 = glm_vec3_dot(plane->norm, vertices[(*to_clipped_hd)[1]].vtx) - plane->d;
            d2 = glm_vec3_dot(plane->norm, vertices[(*to_clipped_hd)[2]].vtx) - plane->d;

            if (d0 != d0 || d1 != d1 || d2 != d2) {
                // d# may be NaN -> skip this tri
                outsides[outside_cnt++] = (*to_clipped_hd)[0];
                outsides[outside_cnt++] = (*to_clipped_hd)[1];
                outsides[outside_cnt++] = (*to_clipped_hd)[2];
            } else {
                if (d0 < 0)
                    outsides[outside_cnt++] = (*to_clipped_hd)[0];
                else
                    insides[inside_cnt++] = (*to_clipped_hd)[0];

                if (d1 < 0)
                    outsides[outside_cnt++] = (*to_clipped_hd)[1];
                else
                    insides[inside_cnt++] = (*to_clipped_hd)[1];

                if (d2 < 0)
                    outsides[outside_cnt++] = (*to_clipped_hd)[2];
                else
                    insides[inside_cnt++] = (*to_clipped_hd)[2];
            }

            switch (inside_cnt) {
            case 0: // outside_cnt == 3
                vertices[outsides[0]].used = vertices[outsides[1]].used = vertices[outsides[2]].used = 0;
                break;  // 0 new tri

            case 1: // outside_cnt == 2
                vertices[outsides[0]].used = vertices[outsides[1]].used = 0;

                (*to_clipped_tail)[0] = insides[0];

                vec_intersect_plane(plane, vertices[insides[0]].vtx, vertices[outsides[0]].vtx, vt);
                (*to_clipped_tail)[1] = push_back_helper_buf_vtx(bgl, vt) - ibuf->v_off;

                vec_intersect_plane(plane, vertices[insides[0]].vtx, vertices[outsides[1]].vtx, vt);
                (*to_clipped_tail++)[2] = push_back_helper_buf_vtx(bgl, vt) - ibuf->v_off;

                break;  // 1 new tri

            case 2: // outside_cnt == 1
                vertices[outsides[0]].used = 0;

                (*to_clipped_tail)[0] = insides[0];
                (*to_clipped_tail)[1] = insides[1];
                vec_intersect_plane(plane, vertices[insides[0]].vtx, vertices[outsides[0]].vtx, vt);
                (*to_clipped_tail++)[2] = push_back_helper_buf_vtx(bgl, vt) - ibuf->v_off;

                (*to_clipped_tail)[0] = insides[1];
                (*to_clipped_tail)[1] = to_clipped_tail[-1][2];
                vec_intersect_plane(plane, vertices[insides[1]].vtx, vertices[outsides[0]].vtx, vt);
                (*to_clipped_tail++)[2] = push_back_helper_buf_vtx(bgl, vt) - ibuf->v_off;

                break;  // 2 new tri

            case 3: // outside_cnt == 0
                glm_ivec3_copy(*to_clipped_hd, *to_clipped_tail++);
                break;  // 1 new tri

            default:    // unreached
                break;
            }
        }
    }

    *start = to_clipped_hd;
    *end = to_clipped_tail;
}

void draw_buffers(bgl_instance bgl, mat4 vp) {
    VHB_INIT(bgl, vhb);
    IHB_INIT(bgl, ihb);
    CHB_INIT(bgl, chb);
    idx_item *cbuf = chb->buf;
    ivec3 clipped[64], *clip, *clipped_end;
    vertex_item *vertices;

    if ((chb->buf_sz < ihb->buf_sz || !cbuf)
            && !(cbuf = chb->buf = realloc(cbuf, (chb->buf_sz = ihb->buf_sz) * sizeof(*cbuf))))
        return;

    // transform to view then project space
    for (vertex_item *vxi = vhb->buf; vxi < &((vertex_item *)vhb->buf)[vhb->cnt]; ++vxi)
        if (vxi->used)
            loc_to_dev(vp, vxi->vtx, vxi->vtx);

    int i = ihb->cnt;
    for (idx_item *ibuf = ihb->buf; i--; ++ibuf) {
        vertices = &((vertex_item *)vhb->buf)[ibuf->v_off];

        switch (ibuf->n) {
        case 1:
            {
                vec4 *p = &vertices[ibuf->tri[0]].vtx;
                if ((*p)[0] >= -1 && (*p)[0] <= 1
                        && (*p)[1] >= -1 && (*p)[1] <= 1
                        && (*p)[2] >= -1 && (*p)[2] <= 1) {
                    ibuf->avg_z = vertices[ibuf->tri[0]].vtx[2];
                    cbuf[chb->cnt++] = *ibuf;
                }
            }
            break;

        case 2:
            // TODO: clip line
            break;

        case 3:
            clip_tri(bgl, ibuf, clipped, &clip, &clipped_end);
            if (clip != clipped_end) {
                if (chb->cnt + (clipped_end - clip) >= chb->buf_sz
                        && !(cbuf = chb->buf = realloc(cbuf, (chb->buf_sz <<= 1) * sizeof(*cbuf))))
                    return;

                for (; clip < clipped_end; ++clip) {
                    cbuf[chb->cnt] = *ibuf;
                    glm_ivec3_copy(*clip, cbuf[chb->cnt].tri);
                    cbuf[chb->cnt++].avg_z = (vertices[(*clip)[0]].vtx[2] + vertices[(*clip)[1]].vtx[2] + vertices[(*clip)[2]].vtx[2]) / 3.0f;
                    vertices[(*clip)[0]].used = vertices[(*clip)[1]].used = vertices[(*clip)[2]].used = 1;
                }
            }
            break;

        default:
            break;
        }
    }

    qsort(cbuf, chb->cnt, sizeof(*cbuf), (cmp_fn_t)cmp_idx);

    // convert to framebuffer coordinates
    for (vertex_item *vxi = vhb->buf; vxi < &((vertex_item *)vhb->buf)[vhb->cnt]; ++vxi)
        if (vxi->used)
//            dev_to_fb(&bgl->viewport, vxi->vtx, vxi->vtx);
            dev_to_fbi(&bgl->viewport, vxi->vtx, vxi->ivtx);

//    vec4 wires = GLM_VEC4_ONE_INIT;

    i = chb->cnt;
    for (; i--; ++cbuf) {
        vertices = &((vertex_item *)vhb->buf)[cbuf->v_off];

        switch (cbuf->n) {
        case 1:
            bgl->dev.draw_pixel(bgl, vertices[cbuf->tri[0]].ivtx, cbuf->color);
            break;
        case 2:
            bgl->dev.draw_line(bgl,
                               vertices[cbuf->tri[0]].ivtx,
                               vertices[cbuf->tri[1]].ivtx,
                               cbuf->color);
            break;
        case 3:
            bgl->dev.draw_fill_triangle(bgl,
                                        vertices[cbuf->tri[0]].ivtx,
                                        vertices[cbuf->tri[1]].ivtx,
                                        vertices[cbuf->tri[2]].ivtx,
                                        cbuf->color);

            /*  TODO: set drawing mode (points, lines (wireframes), fill)
            bgl->dev.draw_triangle(bgl,
                                   vertices[cbuf->tri[0]].ivtx,
                                   vertices[cbuf->tri[1]].ivtx,
                                   vertices[cbuf->tri[2]].ivtx,
                                   wires);
            //*/
            break;
        }
    }

    ihb->cnt = vhb->cnt = chb->cnt = 0;
}
