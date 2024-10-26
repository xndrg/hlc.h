/*
Inspired by Nobuild project created by Alexey Kutepov and released under MIT license.
https://github.com/tsoding/nobuild
I took some concepts from it.
This library is created to be used in personal projects.
TODO:
    1. String types.
    2. Basic data structures (Dynamic array, Associative array)
*/
#ifndef HLC_H_
#define HLC_H_

#include <assert.h>
#include <stdlib.h>
#include <errno.h>
#include <stdbool.h>
#include <string.h>

#define HLC_UNUSED(value) (void)(value)
#define HLC_TODO(msg) do { fprintf(stderr, "%s:%d: TODO %s\n", __FILE__, __LINE__, msg); abort(); } while(0)
#define HLC_UNREACHABLE(msg) do { fprintf(stderr, "%s:%d UNREACHABLE: %s\n", __FILE__, __LINE__, msg); abort(); } while(0)

#define HLC_ARRAY_LEN(array) (sizeof(array) / sizeof(array[0]))
#define HLC_ARRAY_GET(array, index) \
(assert((size_t)index < HLC_ARRAY_LEN(array)), array[(size_t)index])

#define hlc_return_defer(value) do { result = (value); goto defer; } while(0)

typedef struct {
    char  *data;
    size_t size;
} Hlc_File_Buffer;

bool hlc_read_file(const char *file_path, Hlc_File_Buffer *str);
#define hlc_fb_free(fb) free((fb).data)

#ifdef HLC_IMPLEMENTATION

bool hlc_read_file(const char *file_path, Hlc_File_Buffer *str)
{
    bool result = true;

    FILE *f = fopen(file_path, "rb");
    if (f == NULL)                 hlc_return_defer(false);
    if (fseek(f, 0, SEEK_END) < 0) hlc_return_defer(false);
    long m = ftell(f);
    if (m < 0)                     hlc_return_defer(false);
    if (fseek(f, 0, SEEK_SET) < 0) hlc_return_defer(false);

    size_t len = m;
    str->data = malloc(sizeof(char) * len);
    assert(str->data != NULL && "Buy more RAM lool!!");

    fread(str->data, m, 1, f);
    if (ferror(f))                 hlc_return_defer(false);
    str->size = len;

defer:
    if (!result) fprintf(stderr, "Could not read file %s: %s", file_path, strerror(errno));
    if (f) fclose(f);
    return result;
}

#endif // HLC_IMPLEMENTATION

#endif // HLC_H_
