/*
Inspired by Nobuild project created by Alexey Kutepov and released under MIT license.
https://github.com/tsoding/nobuild
I took some concepts from it and adapted for my cases.
This library is created to be used in personal projects.
*/
#ifndef HLC_H_
#define HLC_H_

#include <stdio.h>
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

/*
// Example of reading content from file using File_Buffer
#include <stdio.h>

#define HLC_IMPLEMENTATION
#include "hlc.h"

int main(void)
{
    const char *file_path = "hello.bf";
    File_Buffer fb = read_entire_file(file_path);

    printf("Read %zu bytes\n", fb.size);
    // File buffer is null-terminated
    printf("%s\n", fb.data);
    
    return 0;
}
*/

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

#ifndef DA_INIT_CAP
#define DA_INIT_CAP 256
#endif

/*
// Example of defining and using dynamic array
#include <stdio.h>

#define HLC_IMPLEMENTATION
#include "hlc.h"

typedef struct {
    int *items;
    size_t len;
    size_t cap;
} Nums;

int main(void)
{
    Nums nums = {0};

    da_append(&nums, 1);
    da_append(&nums, 2);
    da_append(&nums, 3);

    int many[] = {4, 5, 6, 7, 8, 9};
    da_append_many(&nums, many, sizeof(many)/sizeof(many[0]));
    for (int i = 0; i < nums.len; ++i) {
	printf("%d ", nums.items[i]);
    }

    // Free memory after use
    da_free(nums);
    return 0;
}
*/

#define da_free(da) free((da).items)

#define da_append(da, item)                                                     \
    do {                                                                        \
	if ((da)->len >= (da)->cap) {                                           \
	    (da)->cap = (da)->cap == 0 ? DA_INIT_CAP : (da)->cap*2;             \
	    (da)->items = realloc((da)->items, (da)->cap*sizeof(*(da)->items)); \
	    assert((da)->items != NULL && "Buy more RAM lool!!");               \
	}                                                                       \
                                                                                \
	(da)->items[(da)->len++] = (item);                                      \
    } while(0)

#define da_append_many(da, new_items, new_items_count)                                        \
    do {                                                                                      \
	if ((da)->len + (new_items_count) > (da)->cap) {                                      \
	    if ((da)->cap == 0) {                                                             \
		(da)->cap = DA_INIT_CAP;                                                      \
	    }                                                                                 \
	    while ((da)->len + (new_items_count) > (da)->cap) {                               \
		(da)->cap *= 2;                                                               \
	    }                                                                                 \
	    (da)->items = realloc((da)->items, (da)->cap*sizeof(*(da)->items));               \
	    assert((da)->items != NULL && "Buy more RAM lool!!");                             \
	}                                                                                     \
	memcpy((da)->items + (da)->len, (new_items), (new_items_count)*sizeof(*(da)->items)); \
	(da)->len += (new_items_count);                                                       \
    } while(0)

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
