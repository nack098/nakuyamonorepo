#include <stdlib.h>
#include <string.h>

#include "array-list.h"

static inline array_list_data_t *array_list_get_data(const array_list_t arr) {
    array_list_data_t *data = NULL;
    data = (array_list_data_t *)&arr[-sizeof(*data)];
    return data;
}

array_list_t array_list_create(size_t length, size_t element_size) {
    array_list_data_t *data = NULL;
    array_list_t *arr = NULL;
    size_t allocation = sizeof(*data) + length * element_size;

    data = (array_list_data_t *)malloc(allocation);

    data->element_size = element_size;
    data->length = 0;
    data->allocation = allocation;

    memset(data->data, 0, allocation - sizeof(*data));

    arr = (array_list_t *)data->data;
    return arr;

_exit:
    return arr;
}

array_list_t array_list_create_from_c_array(void *data, size_t element_size) {
    array_list_t *arr = NULL;
    arr = array_list_create(sizeof(data) / element_size, element_size);
    if (arr != NULL)
        memcpy_s(arr, sizeof(data), data, sizeof(data));
    return arr;
}

void array_list_free(array_list_t *arr) {
    if (arr == NULL | *arr == NULL)
        return;

    free(array_list_get_data(*arr));
    *arr = NULL;
}

void array_list_append(array_list_t *arr, void **data) {
    array_list_data_t *arr_data = NULL;
    arr_data = array_list_get_data(*arr);

    if (arr_data->allocation < sizeof(*arr_data) +
                                   arr_data->element_size * arr_data->length +
                                   arr_data->element_size) {
        array_list_resize(arr, (arr_data->length + 1) * 1.5);
        arr_data = array_list_get_data(*arr);
    }

    memcpy_s(arr_data->data + arr_data->element_size * arr_data->length,
             arr_data->element_size, data, arr_data->element_size);

    ++arr_data->length;
_exit:
    return;
}

void array_list_resize(array_list_t *arr, size_t length) {
    array_list_data_t *arr_data = NULL, *new_arr = NULL;
    arr_data = array_list_get_data(*arr);
    new_arr =
        realloc(arr_data, sizeof(*arr_data) + arr_data->element_size * length);

    if (new_arr != NULL) {
        new_arr->allocation = sizeof(*new_arr) + new_arr->element_size * length;
        *arr = new_arr->data;
    }

_exit:
    return;
}

void array_list_clear(array_list_t *arr) {
    array_list_data_t *data = array_list_get_data(*arr);
    data->length = 0;
    *arr = data->data;
}

size_t array_list_length(const array_list_t arr) {
    return array_list_get_data(arr)->length;
}

size_t array_list_element_size(const array_list_t arr) {
    return array_list_get_data(arr)->element_size;
}
