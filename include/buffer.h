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
    void* data;
    size_t position;
    size_t bytes_allocated;
    size_t bytes_left;
} buffer_t;

/**
 * @brief Initializes buffer instance
 * @param instance Pointer to buffer instance
 * @return 0 if success, -1 if initialization failed
 */
int buffer_init(buffer_t* const instance);

/**
 * @brief Increases size of buffer instance
 * @param instance Pointer to buffer instance
 * @param increments Value to increase buffer size by (in units of BUFFER_REALLOC_INCREMENT)
 * @return 0 if success, -1 if expansion failed
 */
int buffer_expand(buffer_t* const instance, size_t increments);

/**
 * @brief Appends new content to buffer instance
 * @param instance Pointer to buffer instance
 * @param data Data to be appended
 * @param size Size of the data to be appended (in bytes)
 * @return 0 if success, -1 if appending failed
 */
int buffer_append(buffer_t* const instance, const void* const data, size_t size);

/**
 * @brief Clears buffer instance (but doesn't resize it to BUFFER_INITIAL_SIZE!)
 * @param instance Pointer to buffer instance
 */
void buffer_reset(buffer_t* const instance);

/**
 * @brief Deinitalizes buffer instance
 * @param instance Pointer to buffer instance
 */
void buffer_deinit(buffer_t* const instance);

#endif
