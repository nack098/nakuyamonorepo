#pragma once

/*
 *  Struct Allocation
 *  | header | data | \n |
 */

typedef struct {
    size_t allocation, length;
    char data[];
} string_data_t;

typedef struct {
    size_t length;
    char *location;
} string_view_t;

typedef char *string_t;

string_data_t *string_get_data(string_t str);

string_t string_create(size_t size);
string_t string_from_cstr(const char *str);
string_t string_from_view(string_view_t view);

void string_append(string_t *to, const char *from);
void string_append_char(string_t *to, const char from);
void string_set(string_t *to, const char *from);
void string_resize(string_t *str, size_t size);
void string_clear(string_t *str);
void string_free(string_t *str);

size_t string_length(string_t str);

string_view_t string_slice(string_t str, size_t start, size_t end);
string_view_t *string_split(string_t str, const char *delim);

int string_view_compare(string_view_t a, string_view_t b);
int string_view_compare_str(string_view_t a, const char *b);

void string_view_print(string_view_t view);
