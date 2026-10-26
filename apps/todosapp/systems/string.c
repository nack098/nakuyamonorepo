#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "array-list.h"
#include "string.h"

string_data_t *string_get_data(string_t str) {
    string_data_t *data;
    data = (string_data_t *)&str[-sizeof(*data)];
    return data;
}

string_t string_create(size_t size) {
    string_data_t *data = NULL;
    size_t allocation = sizeof(*data) + size + 1;

    data = (string_data_t *)malloc(allocation);

    *data = (string_data_t){0};
    data->allocation = allocation;
    data->length = 0;
    data->data[data->length] = '\0';

    return data->data;

_cleanup:
    if (data != NULL)
        free(data);
    return NULL;
}

string_t string_from_cstr(const char *c_str) {
    size_t length = 0;
    string_t str = NULL;

    length = strlen(c_str);
    str = string_create(length);
    string_get_data(str)->length = length;
    memcpy_s(str, length, c_str, length);
    str[length] = '\0';

    return str;
}

string_t string_from_view(string_view_t view) {
    size_t length = 0;
    string_t str = NULL;

    length = view.length;
    str = string_create(length);
    string_get_data(str)->length = length;
    memcpy_s(str, length, view.location, length);
    str[length] = '\0';

    return str;
}

void string_set(string_t *to, const char *from) {
    size_t length = 0;
    string_data_t *str = NULL;
    string_data_t *new_ptr = NULL;

    str = string_get_data(*to);
    length = strlen(from);

    if (str->allocation < sizeof(*str) + length + 1) {
        new_ptr = realloc(str, sizeof(*str) + length + 1);
        if (new_ptr != NULL)
            str = new_ptr;
        str->allocation = sizeof(*str) + length + 1;
    }
    memcpy_s(str->data, length, from, length);
    str->data[length] = '\0';
    str->length = length;
    *to = str->data;
}

void string_append(string_t *to, const char *from) {
    size_t length = 0, old_length = 0, str_len = 0;
    string_data_t *str = NULL;
    string_data_t *new_ptr = NULL;

    str = string_get_data(*to);
    str_len = strlen(from);
    old_length = str->length;
    length = old_length + str_len;

    if (str->allocation < sizeof(*str) + length + 1) {
        new_ptr = realloc(str, sizeof(*str) + length + 1);
        if (new_ptr != NULL)
            str = new_ptr;
        str->allocation = sizeof(*str) + length + 1;
    }

    memcpy_s(str->data + old_length, str_len, from, str_len);
    str->data[length] = '\0';
    str->length = length;
    *to = str->data;
}

void string_append_char(string_t *to, const char from) {
    size_t length = 0, old_length = 0, str_len = 0;
    string_data_t *str = NULL;
    string_data_t *new_ptr = NULL;

    str = string_get_data(*to);
    str_len = 1;
    old_length = str->length;
    length = old_length + str_len;

    if (str->allocation < sizeof(*str) + length + 1) {
        new_ptr = realloc(str, sizeof(*str) + length + 1);
        if (new_ptr != NULL)
            str = new_ptr;
        str->allocation = sizeof(*str) + length + 1;
    }
    memcpy_s(str->data + old_length, str_len, &from, str_len);
    str->data[length] = '\0';
    str->length = length;
    *to = str->data;
}

void string_resize(string_t *str, size_t size) {
    string_data_t *old_str = NULL, *new_str = NULL;
    size_t old_length = 0, allocation = 0;

    if (str == NULL || *str == NULL)
        return;

    old_str = string_get_data(*str);
    old_length = old_str->length;

    if (size > SIZE_MAX - sizeof(*old_str) - 1)
        return;

    allocation = sizeof(*old_str) + size + 1;
    new_str = realloc(old_str, allocation);

    if (new_str == NULL)
        return;

    new_str->allocation = allocation;
    new_str->length = old_length > size ? size : old_length;
    new_str->data[new_str->length] = '\0';

    *str = new_str->data;
}

void string_clear(string_t *str) {
    string_data_t *data = string_get_data(*str);
    data->length = 0;
    data->data[0] = '\0';
    *str = data->data;
}

size_t string_length(string_t str) { return string_get_data(str)->length; }

string_view_t string_slice(string_t str, size_t start, size_t end) {
    size_t length =
        end <= string_length(str) ? end - start : string_length(str) - start;
    assert(start <= end);
    return (string_view_t){.length = length,
                           .location = &(string_get_data(str)->data[start])};
}

string_view_t *string_split(string_t str, const char *delim) {
    size_t str_len = string_length(str);
    size_t delim_len = strlen(delim);
    size_t cnt = 0, cnt2 = 0, start = 0;
    string_view_t *arr, tmp = {0};
    int check = 0;
    assert(str_len >= delim_len);
    assert(delim != NULL && delim_len != 0);

    if (str_len == delim_len)
        return strcmp(str, delim) == 0 ? 0 : NULL;

    arr = array_list_create(0, sizeof(string_view_t));

    for (cnt = 0; cnt < str_len - delim_len; ++cnt) {
        if (str[cnt] == delim[0] &&
            str[cnt + delim_len - 1] == delim[delim_len - 1]) {
            check = 1;
            for (cnt2 = 1; cnt2 < delim_len - 1; ++cnt2) {
                if (str[cnt + cnt2] != delim[cnt2]) {
                    check = 0;
                    break;
                }
            }
            if (check) {
                if (cnt - start != 0) {
                    tmp.length = cnt - start;
                    tmp.location = str + start;
                    array_list_append((array_list_t *)&arr, (void **)(&tmp));
                }
                start = cnt + delim_len;
                cnt = start;
            }
        }
    }

    if (start != str_len) {
        tmp.length = str_len - start;
        tmp.location = str + start;
        array_list_append((array_list_t *)&arr, (void **)(&tmp));
    }
    return arr;
}

void string_free(string_t *str) {
    if (str == NULL || *str == NULL)
        return;

    free(string_get_data(*str));
    *str = NULL;
}

int string_view_compare(string_view_t a, string_view_t b) {
    int result = 1;
    size_t cnt = 0;

    if (a.length != b.length) {
        result = 0;
        goto _exit;
    }

    for (cnt = 0; cnt < a.length; ++cnt)
        if (a.location[cnt] != b.location[cnt]) {
            result = 0;
            goto _exit;
        }
_exit:
    return result;
}

int string_view_compare_str(string_view_t a, const char *b) {
    int result = 1;
    size_t b_length = 0, cnt = 0;

    b_length = strlen(b);

    if (a.length != b_length) {
        result = 0;
        goto _exit;
    }

    for (cnt = 0; cnt < a.length; ++cnt) {
        if (a.location[cnt] != b[cnt]) {
            result = 0;
            goto _exit;
        }
    }

_exit:
    return result;
}

void string_view_print(string_view_t view) {
    for (int i = 0; i < view.length; ++i) {
        putchar(view.location[i]);
    }
}
