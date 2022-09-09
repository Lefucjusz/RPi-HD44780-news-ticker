/*
 * parser.h
 *
 *  Created on: 07.09.2022
 *      Author: Lefucjusz
 */

#ifndef __PARSER_H__
#define __PARSER_H__

#include "buffer.h"

/**
 * @brief Creates single string in proper format that can be displayed on HD44780
 *        display in a form of moving row from raw XML data.
 * @param stream String with raw XML data
 * @param result Formatted string ready to display
 * @return TODO - 0 if parsing succeded, error code if failed
 */
void parser_parse(const buffer_t* const stream, buffer_t* const result);

#endif
