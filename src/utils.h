/*
 * Copyright (c) 2022 Alexander Baskikh
 *
 * MIT License (MIT), http://opensource.org/licenses/MIT
 * Full license can be found in the LICENSE file
 */

#ifndef BGL_UTILS_H
#define BGL_UTILS_H

#include <stdint.h>
#include <stdlib.h>
#include <string.h>


#if defined _WIN32 || defined __CYGWIN__
#else
#endif


BGL_INLINE void *bgl_aligned_alloc(size_t alignment, size_t size) {
#if defined _WIN32 || defined __CYGWIN__
    return _aligned_malloc(size, alignment);
#else
    return aligned_alloc(alignment, size);
#endif
}

BGL_INLINE void bgl_aligned_free(void *ptr) {
#if defined _WIN32 || defined __CYGWIN__
    _aligned_free(ptr);
#else
    free(ptr);
#endif
}

BGL_INLINE void *bgl_realloc_array(void *ptr, size_t n, size_t size) {
#if defined _WIN32 || defined __CYGWIN__
    return realloc(ptr, n * size);
#else
# if (__GLIBC_MINOR__ >= 26)
    return reallocarray(ptr, n, size);
# else
    return realloc(ptr, n * size);
# endif
#endif
}

BGL_INLINE ssize_t bgl_getline(char **lineptr, int *n, FILE *stream) {
#if defined _WIN32 || defined __CYGWIN__
    errno = 0;
    if (fgets(*lineptr, *n, stream))
        return (ssize_t)strlen(*lineptr);
    return -1;
#else
    return getline(lineptr, n, stream);
#endif
}

BGL_INLINE char *bgl_strndup(const char *string, size_t n) {
#if defined _WIN32 || defined __CYGWIN__
    char *ret = strdup(string);
    ret[n] = 0;
    return ret;
#else
    return strndup(string, n);
#endif
}

#endif // BGL_UTILS_H
