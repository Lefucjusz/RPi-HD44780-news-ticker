/*
 * buffer.c
 *
 *  Created on: 06.09.2022
 *      Author: Lefucjusz
 */

#include "buffer.h"
#include <stdlib.h>
#include <string.h>

#define BUFFER_INITIAL_SIZE 4096 // bytes
#define BUFFER_REALLOC_INCREMENT 1024 // bytes

int buffer_init(buffer_t* const instance) {
    instance->data = malloc(BUFFER_INITIAL_SIZE);
    instance->position = 0;

    if(instance->data == NULL) {
        instance->bytes_allocated = 0;
        instance->bytes_left = 0;
        return -1;
    }

    instance->bytes_allocated = BUFFER_INITIAL_SIZE;
    instance->bytes_left = BUFFER_INITIAL_SIZE;
    return 0;
}

int buffer_expand(buffer_t* const instance, size_t increments) {
     size_t size_increment = BUFFER_REALLOC_INCREMENT * increments;
     size_t new_size = instance->bytes_allocated + size_increment;
     void* new_data = realloc(instance->data, new_size);

     if(new_data == NULL) {
        return -1;
     }

     instance->data = new_data;
     instance->bytes_allocated += size_increment;
     instance->bytes_left += size_increment;
     return 0;
}

int buffer_append(buffer_t* const instance, const void* const data, size_t size) {
    /* Get length of data */
    /* WARNING - UNSAFE, data might be not null-terminated or even not a string */
    if(size == 0) {
        size = strlen(data);
    }
    /* Data won't fit, expand buffer */
    if(size > instance->bytes_left) {
        /* Get full increments value */
        size_t bytes_to_expand = size - instance->bytes_left;
        size_t size_increments = bytes_to_expand / BUFFER_REALLOC_INCREMENT;

        /* If remainder present, get one increment more */
        if(bytes_to_expand % BUFFER_REALLOC_INCREMENT) {
            size_increments++;
        }

        /* Expand buffer */
        int error = buffer_expand(instance, size_increments);
        if(error) {
            return -1; // Expansion failed
        }
    }

    /* Copy data and update internal state variables */
    memcpy((char*)instance->data + instance->position, data, size);
    instance->position += size;
    instance->bytes_left -= size;
    return 0;
}

void buffer_reset(buffer_t* const instance) {
    instance->bytes_left = instance->bytes_allocated;
    instance->position = 0;
}

void buffer_deinit(buffer_t* const instance) {
    free(instance->data);
    memset(instance, 0, sizeof(*instance));
}
