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
#include <ctype.h>

#define UNUSED(value) (void)(value)
#define TODO(msg) do { fprintf(stderr, "%s:%d: TODO %s\n", __FILE__, __LINE__, msg); abort(); } while(0)
#define UNREACHABLE(msg) do { fprintf(stderr, "%s:%d UNREACHABLE: %s\n", __FILE__, __LINE__, msg); abort(); } while(0)

#define ARRAY_LEN(array) (sizeof(array) / sizeof(array[0]))

typedef struct {
    char  *data;
    size_t size;
} File_Buffer;

File_Buffer read_entire_file(const char *file_path);
#define fb_free(fb) free((fb).data)

typedef struct {
    const char *data;
    size_t size;
} String_View;

typedef bool(*String_View_Predicate)(char);

#define SV_LITERAL(__literal__) (String_View) { .data = (__literal__), .size = sizeof(__literal__) - 1}

#define SV_Fmt "%.*s"
#define SV_Arg(__sv__) (int) (__sv__).size, (__sv__).data

String_View sv_new(const char *data, size_t size);
bool        sv_equals(const String_View a, const String_View b);
bool        sv_has_prefix(String_View sv, String_View prefix);
String_View sv_chop(String_View *sv, size_t n);
String_View sv_chop_prefix(String_View *sv, String_View prefix);
String_View sv_chop_while(String_View *sv, String_View_Predicate predicate);
bool        sv_is_alpha(const char c);

#ifdef HLC_IMPLEMENTATION

File_Buffer read_entire_file(const char *file_path)
{
    File_Buffer fb = {0};

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

String_View sv_new(const char *data, size_t size)
{
    return (String_View) {
	.data = data,
	.size = size,
    };
}

bool sv_equals(const String_View a, const String_View b)
{
    if (a.size != b.size) {
	return false;
    }

    return memcmp(a.data, b.data, a.size) == 0;
}

bool sv_has_prefix(String_View sv, String_View prefix)
{
    if (sv.size < prefix.size) {
	return false;
    }

    String_View substr = sv_new(sv.data, prefix.size);
    return sv_equals(substr, prefix);
}

String_View sv_chop(String_View *sv, size_t n)
{
    if (sv->size < n) {
	sv->size = n;
    }

    String_View result = sv_new(sv->data, n);

    sv->size = sv->size - n;
    sv->data = sv->data + n;

    return result;
}

String_View sv_chop_prefix(String_View *sv, String_View prefix)
{
    if (!sv_has_prefix(*sv, prefix)) {
	return sv_new(NULL, 0);
    }

    return sv_chop(sv, prefix.size);
}

String_View sv_chop_while(String_View *sv, String_View_Predicate predicate)
{
    size_t i = 0;
    
    while (i < sv->size && predicate(sv->data[i])) {
	i += 1;
    }

    return sv_chop(sv, i);
}

bool sv_is_alpha(const char c) {
    return (bool) isalpha((int) c);
}

#endif // HLC_IMPLEMENTATION

#endif // HLC_H_
