/*
 * parser.h
 *
 *  Created on: 07.09.2022
 *      Author: Lefucjusz
 */

#ifndef __PARSER_H__
#define __PARSER_H__

#include "buffer.h"

void parser_parse(const buffer_t* const stream, buffer_t* const result);

#endif
