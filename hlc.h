/*
Inspired by Nobuild project created by Alexey Kutepov and released under MIT license.
https://github.com/tsoding/nobuild
I took some concepts from it and adapted for my cases.
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

Hlc_File_Buffer hlc_read_entire_file(const char *file_path);
#define hlc_fb_free(fb) free((fb).data)

#ifdef HLC_IMPLEMENTATION

Hlc_File_Buffer hlc_read_entire_file(const char *file_path)
{
    Hlc_File_Buffer fb = {0};

    FILE *f = fopen(file_path, "rb");
    if (f == NULL)                 goto defer;
    if (fseek(f, 0, SEEK_END) < 0) goto defer;
    fb.size = ftell(f);
    if (fb.size < 0)               goto defer;
    if (fseek(f, 0, SEEK_SET) < 0) goto defer;

    fb.data = malloc(fb.size + 1);
    assert(fb.data != NULL && "Buy more RAM lool!!");

    fread(fb.data, fb.size, 1, f);
    if (ferror(f))                 goto defer;
    fb.data[fb.size] = '\0';

defer:
    if (f) fclose(f);
    return fb;
}

#endif // HLC_IMPLEMENTATION

#endif // HLC_H_
