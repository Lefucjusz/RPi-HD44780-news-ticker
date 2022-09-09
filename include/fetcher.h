/*
 * fetcher.h
 *
 *  Created on: 08.09.2022
 *      Author: Lefucjusz
 */

#ifndef __FETCHER_H__
#define __FETCHER_H__

#include "buffer.h"

/**
 * @brief Fetches raw XML from TVN24 news API
 * @param fetched_data Pointer to buffer instance to save fetched data in
 * @return 0 if success, -1 if fetching failed
 */
int fetcher_fetch(buffer_t* const fetched_data);

#endif
