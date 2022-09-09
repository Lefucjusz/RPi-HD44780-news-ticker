/*
 * display.h
 *
 *  Created on: 07.09.2022
 *      Author: Lefucjusz
 */

#ifndef __DISPLAY_H__
#define __DISPLAY_H__

#include "GPIO.h"
#include "HD44780.h"

#define DISPLAY_COL_NUM 20

typedef enum {
    DISPLAY_ROW_TOP = 0,
    DISPLAY_ROW_BOTTOM,
    DISPLAY_ROW_NUM
} display_row_t;

/* 
 * Starting the enum from value 0x08 is a little trick
 * enabling usage of HD44780's custom CGRAM at address 0x00.
 * In ASCII, value 0x00 has a special function - it's
 * a null-terminator, so it cannot be used to represent
 * "normal" character.
 * Fortunately, HD44780 has custom CGRAM mapped to two
 * areas - 0x00-0x07 and 0x08-0x0F, so using second
 * one solves the issue.
 * Note - this doesn't mean that HD44780 supports
 * 16 CGRAM characters! Only 8 are supported, but
 * each is mapped to two different addresses.
 */
typedef enum {
    DISPLAY_POLISH_A = 0x08,
    DISPLAY_POLISH_C,
    DISPLAY_POLISH_E,
    DISPLAY_POLISH_L,
    DISPLAY_POLISH_N,
    DISPLAY_POLISH_O,
    DISPLAY_POLISH_S,
    DISPLAY_POLISH_Z
} display_diacritics_t;

/**
 * @brief Substitutes polish diacritics for the corresponding HD44780 CGRAM addresses
 * @param stream String to replace diacritics in
 */
void display_substitute_diacritics(char* const stream);

/**
 * @brief Initializes display
 * @param gpio Raspberry PI GPIO driver handle
 * @param config Struct with HD44780 display configuration
 */
void display_init(gpio_t gpio, HD44780_config_t* config);

/**
 * @brief Updates moving row - moves all characters one place left, discards the leftmost,
 *        appends new at the end
 *
 * @param new_char Character to be appended
 * @param row Display row to be updated
 */
void display_update_row(char new_char, display_row_t row);

/**
 * @brief Displays string in row
 * @param string String to be displayed
 * @param row Display row to show string in
 */
void display_string_row(const char* const string, display_row_t row);

#endif
