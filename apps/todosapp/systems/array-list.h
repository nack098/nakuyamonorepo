#pragma once

#include <stdint.h>

typedef struct {
    size_t allocation;
    size_t element_size;
    size_t length;
    uint8_t data[];
} array_list_data_t;

typedef void *array_list_t;

array_list_t array_list_create(size_t length, size_t element_size);
array_list_t array_list_create_from_c_array(void *data, size_t element_size);
void array_list_free(array_list_t *arr);

void array_list_append(array_list_t *arr, void **data);
void array_list_resize(array_list_t *arr, size_t length);
void array_list_clear(array_list_t *arr);

size_t array_list_length(const array_list_t arr);
size_t array_list_element_size(const array_list_t arr);
