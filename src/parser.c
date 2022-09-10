/*
 * parser.c
 *
 *  Created on: 07.09.2022
 *      Author: Lefucjusz
 */

#include "parser.h"
#include <stddef.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>

/*
 * Used to create news number indicator.
 * 2 chars for item number,
 * 1 char for '/',
 * 2 chars for number of items,
 * 1 char for null-terminator.
 * There should never be more than 99 items I suppose :P
 */
#define PARSER_NUMBER_BUFFER_SIZE 6

/* Number of whitechars after img src closing tag */
#define PARSER_SPACE_AFTER_IMG_SRC_END_TAG 5

/* Used to parse last update time */
#define PARSER_DATE_LENGTH 10 // YYYY-MM-DD
#define PARSER_TIME_LENGTH 8 // HH:MM:SS

static size_t get_tag_content(const char* const stream, const char* const tag, const char** const content) {
    /* Search for the tag */
    const char* start = strstr(stream, tag);

    /* If tag not found - set content to NULL and return zero length */
    if(start == NULL) {
        *content = NULL;
        return 0;
    }

    /* Obtain position of the beginning of the content - skip tag */
    start += strlen(tag);

    /* If any whitespace chars at the beginning of the content - skip them */
    while(isspace(*start)) {
        start++;
    }

    /* Check if content in CDATA */
    const char* const cdata_str = "<![CDATA[";
    bool is_cdata = false;
    if(strstr(start, cdata_str) == start) {
        is_cdata = true;
        start += strlen(cdata_str);
    }

    /* Obtain position of the end of the content */
    const char* end = NULL;
    if(is_cdata) {
        end = strchr(start, ']'); // Look for end of CDATA
    } else {
        end = strstr(start, "</"); // Look for the ending tag
    }

    /* If tag not found - set content to NULL and return zero length */
    if(end == NULL) {
        *content = NULL;
        return 0;
    }

    /* Move from detected tag to previous char */
    end--;

    /* If any whitespace chars at the end of the content - skip them */
    while(isspace(*end)) {
        end--;
    }

    /* Set content pointer to proper value */
    *content = start;

    /* Return length */
    return end - start + 1;
}

void parser_parse(const buffer_t* const stream, buffer_t* const result) {  //TODO check buffer_append retcodes
    const char* stream_ptr = NULL;
    size_t length = 0;

    /* Append main title */
    length = get_tag_content(stream->data, "<title>", &stream_ptr);
    buffer_append(result, "#", 0);
    buffer_append(result, stream_ptr, length);
    buffer_append(result, "# ostatnia aktualizacja: ", 0);

    /* Append date */
    get_tag_content(stream_ptr, "<lastBuildDate>", &stream_ptr);
    /* Date is in YYYY-MM-DDTHH:MM:SS.000Z format, parse to get YYYY-MM-DD HH:MM:SS format */
    buffer_append(result, stream_ptr, PARSER_DATE_LENGTH);
    buffer_append(result, " ", 0);
    buffer_append(result, stream_ptr + PARSER_DATE_LENGTH + 1, PARSER_TIME_LENGTH); // +1 to skip 'T'
    buffer_append(result, " ", 0);

    /* Get number of items - this is very suboptimal way, but I see no better using this parsing approach */
    int items_number = 0;
    const char* temp_ptr = stream_ptr;
    while(get_tag_content(temp_ptr, "<item>", &temp_ptr) != 0) {
        items_number++;
    }

    /* Append all items - iterate until no more <item> tags left */
    int current_item = 1;
    char number_buffer[PARSER_NUMBER_BUFFER_SIZE];

    while((length = get_tag_content(stream_ptr, "<item>", &stream_ptr)) != 0) {
        /* Append item number */
        snprintf(number_buffer, PARSER_NUMBER_BUFFER_SIZE, "%d/%d", current_item, items_number);
        buffer_append(result, number_buffer, 0);
        buffer_append(result, " -> ", 0);

        /* Append item title */
        length = get_tag_content(stream_ptr, "<title>", &stream_ptr);
        buffer_append(result, stream_ptr, length);
        buffer_append(result, ": ", 0);

        /* Append item description */
        length = get_tag_content(stream_ptr, "<description>", &stream_ptr);
        /* The description is preceded by an image contained in img src tag, skip it */
        const char* const description = strchr(stream_ptr, '>') + PARSER_SPACE_AFTER_IMG_SRC_END_TAG + 1; // Find img src closing tag, skip it and 5 whitechars that are there
        size_t chars_skipped = description - stream_ptr;
        size_t description_length = length - chars_skipped; // Decrease length value by the number of skipped chars

        buffer_append(result, description, description_length);
        buffer_append(result, " ", 0);

        current_item++;
    };
    buffer_append(result, "\0", 0);
}
