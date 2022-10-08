/*
 * display.c
 *
 *  Created on: 07.09.2022
 *      Author: Lefucjusz
 */

#include "display.h"
#include <string.h>
#include <stdint.h>

static const uint8_t diacritics[] = {
    0b00000, 0b01110, 0b00001, 0b01111, 0b10001, 0b01111, 0b00010, 0b00001, // ą
    0b00010, 0b00100, 0b01110, 0b10000, 0b10000, 0b10001, 0b01110, 0b00000, // ć
    0b00000, 0b01110, 0b10001, 0b11111, 0b10000, 0b01110, 0b00100, 0b00010, // ę
    0b01100, 0b00100, 0b00110, 0b00100, 0b01100, 0b00100, 0b01110, 0b00000, // ł
    0b00010, 0b00100, 0b10110, 0b11001, 0b10001, 0b10001, 0b10001, 0b00000, // ń
    0b00010, 0b00100, 0b01110, 0b10001, 0b10001, 0b10001, 0b01110, 0b00000, // ó
    0b00010, 0b00100, 0b01110, 0b10000, 0b01110, 0b00001, 0b11110, 0b00000, // ś
    0b00000, 0b00010, 0b11111, 0b00010, 0b00100, 0b01000, 0b11111, 0b00000  // ż
};

static char rows_buffer[DISPLAY_ROW_NUM][DISPLAY_COL_NUM + 1]; // One additional for null-terminator

/* 
 * Algorithm obtained via reverse-engineering: printing ASCII codes of diacritics... 
 * Because of HD44780 having only 8 user-definable glyphs, it is quite limited.
 * Both minuscules and majuscules are mapped to the same glyph, also both
 * ż and ź are represented as ż.
 */
void display_substitute_diacritics(char* const stream) {
    size_t length = strlen(stream);

    for(size_t i = 0; i < length; i++) {
        switch((int)stream[i]) {
            case 195:
                switch((int)stream[i + 1]) {
                    case 147: // Ó
                    case 179: // o
                        stream[i + 1] = DISPLAY_POLISH_O;
                        break;
                    default:
                        continue;
                }
                break;
            case 196:
                switch((int)stream[i + 1]) {
                    case 132: // Ą
                    case 133: // ą
                        stream[i + 1] = DISPLAY_POLISH_A;
                        break;
                    case 134: // Ć
                    case 135: // ć
                        stream[i + 1] = DISPLAY_POLISH_C;
                        break;
                    case 152: // Ę
                    case 153: // ę
                        stream[i + 1] = DISPLAY_POLISH_E;
                        break;
                    default:
                        continue;
                }
                break;
            case 197: 
                switch((int)stream[i + 1]) {
                    case 129: // Ł
                    case 130: // ł
                        stream[i + 1] = DISPLAY_POLISH_L;
                        break;
                    case 131: // Ń
                    case 132: // ń
                        stream[i + 1] = DISPLAY_POLISH_N;
                        break;
                    case 154: // Ś
                    case 155: // ś
                        stream[i + 1] = DISPLAY_POLISH_S;
                        break;
                    case 185: // Ź
                    case 186: // ź
                    case 187: // Ż
                    case 188: // ż
                        stream[i + 1] = DISPLAY_POLISH_Z;
                        break;
                    default:
                        continue;
                }
                break;
            default:
                continue;
        }

        size_t chars_left = length - i;
        memmove(&stream[i], &stream[i + 1], chars_left); // Every diacritic is coded on 2 bytes - delete the first one
        length--; // Remove that byte from the length too
    }
}

void display_init(gpio_t gpio, HD44780_config_t* config) {
    /* Fill rows buffer with spaces */
    memset(rows_buffer[DISPLAY_ROW_TOP], ' ', DISPLAY_COL_NUM);
    memset(rows_buffer[DISPLAY_ROW_BOTTOM], ' ', DISPLAY_COL_NUM);

    /* Initialize HD44780 controller and load custom glyphs */
    HD44780_init(gpio, config);
    HD44780_load_custom_glyphs(diacritics);
    HD44780_clear();
}

void display_update_row(char new_char, display_row_t row) {
    /* Check if row not out of range */
    if(row > DISPLAY_ROW_BOTTOM) {
        row = DISPLAY_ROW_BOTTOM;
    }
    else if(row < DISPLAY_ROW_TOP) {
        row = DISPLAY_ROW_TOP;
    }

    /* Move all chars in buffer one position left, discard leftmost */
    memmove(rows_buffer[row], rows_buffer[row] + 1, DISPLAY_COL_NUM);

    /* Append new char at the end and terminate string */
    rows_buffer[row][DISPLAY_COL_NUM - 1] = new_char;
    rows_buffer[row][DISPLAY_COL_NUM] = '\0';
    
    /* Display updated buffer */
    HD44780_gotoxy(row + 1, 1); // Array is indexed from 0, rows are indexed from 1
    HD44780_write_string(rows_buffer[row]);
}

void display_string_row(const char* const string, display_row_t row) {
     /* Check if row not out of range */
    if(row > DISPLAY_ROW_BOTTOM) {
        row = DISPLAY_ROW_BOTTOM;
    }
    else if(row < DISPLAY_ROW_TOP) {
        row = DISPLAY_ROW_TOP;
    }

    /* Check if string not too long */
    size_t length = strlen(string);
    if(length > DISPLAY_COL_NUM) {
        length = DISPLAY_COL_NUM;
    }

    /* Load string to buffer and terminate */
    memcpy(rows_buffer[row], string, length);
    rows_buffer[row][DISPLAY_COL_NUM] = '\0';

    /* Display updated buffer */
    HD44780_gotoxy(row + 1, 1); // Array is indexed from 0, rows are indexed from 1
    HD44780_write_string(rows_buffer[row]);
}
