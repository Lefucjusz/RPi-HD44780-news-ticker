/* Created by Lefucjusz, 06.09.2022, Gdańsk */
#ifndef __BUFFER_H__
#define __BUFFER_H__

#include <stddef.h>

typedef struct {
    char* string;
    size_t position;
    size_t chars_allocated;
    size_t chars_left;
} buffer_t;

//TODO doxygen

int buffer_init(buffer_t* instance);
int buffer_expand(buffer_t* instance, size_t increments);
int buffer_append(buffer_t* instance, char* const str, size_t size);
void buffer_deinit(buffer_t* instance);

#endif
