/* Created by Lefucjusz, 06.09.2022, Gdańsk */
#include "buffer.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define BUFFER_INITIAL_SIZE 4096 // chars
#define BUFFER_REALLOC_INCREMENT 1024 // chars

int buffer_init(buffer_t* instance) {
    instance->string = malloc(BUFFER_INITIAL_SIZE * sizeof(*instance->string));
    instance->position = 0;

    if(instance->string == NULL) {
        instance->chars_allocated = 0;
        instance->chars_left = 0;
        return -1;
    }

    instance->chars_allocated = BUFFER_INITIAL_SIZE;
    instance->chars_left = BUFFER_INITIAL_SIZE;
    return 0;
}

int buffer_expand(buffer_t* instance, size_t increments) {
     size_t size_increment = BUFFER_REALLOC_INCREMENT * increments;
     size_t new_size = (instance->chars_allocated + size_increment) * sizeof(*instance->string);
     char* new_string = realloc(instance->string, new_size);

     if(new_string == NULL) {
        return -1;
     }

     instance->string = new_string;
     instance->chars_allocated += size_increment;
     instance->chars_left += size_increment;
     return 0;
}

int buffer_append(buffer_t* instance, char* const str, size_t size) {
    /* Get length from string */
    /* Warning - unsafe, str might be not null-terminated */
    if(size == 0) {
        size = strlen(str);
    }
    /* String won't fit, expand buffer */
    if(size > instance->chars_left) {
	/* Get full increments value */
        size_t chars_to_expand = size - instance->chars_left;
        size_t size_increments = chars_to_expand / BUFFER_REALLOC_INCREMENT;

        /* If remainder present, get one increment more */
        if(chars_to_expand % BUFFER_REALLOC_INCREMENT) {
            size_increments++;
        }

        /* Expand buffer */
        int error = buffer_expand(instance, size_increments);
        if(error) {
            return -1; // Expansion failed
        }
    }

    strncpy(instance->string + instance->position, str, size);
    instance->position += size;
    instance->chars_left -= size;
    return 0;
}

void buffer_deinit(buffer_t* instance) {
    free(instance->string);

    instance->string = NULL;
    instance->position = 0;
    instance->chars_allocated = 0;
    instance->chars_left = 0;
}
