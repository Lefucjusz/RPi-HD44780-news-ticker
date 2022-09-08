/*
 * buffer.h
 *
 *  Created on: 06.09.2022
 *      Author: Lefucjusz
 */
#ifndef __BUFFER_H__
#define __BUFFER_H__

#include <stddef.h>

typedef struct {
    char* data;
    size_t position;
    size_t bytes_allocated;
    size_t bytes_left;
} buffer_t;

//TODO doxygen

int buffer_init(buffer_t* instance);
int buffer_expand(buffer_t* instance, size_t increments);
int buffer_append(buffer_t* instance, const char* const str, size_t size);
void buffer_reset(buffer_t* instance);
void buffer_deinit(buffer_t* instance);

#endif
