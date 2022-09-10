/*
 * main.c
 *
 *  Created on: 01.09.2022
 *      Author: Lefucjusz
 */

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>
#include <stdbool.h>
#include <signal.h>
#include "GPIO.h"
#include "buffer.h"
#include "HD44780.h"
#include "parser.h"
#include "display.h"
#include "fetcher.h"

#define TICKER_REFRESH_DELAY_MS 120

static pthread_mutex_t display_mutex;
static volatile bool running = true;

static void sigint_handler(int signal) {
    running = false;
}

static void* time_task(void* arg) {
    char time_buffer[DISPLAY_COL_NUM + 1]; // One additional for null-terminator
    time_t timestamp;
    struct tm* time_info;

    while(running) {
        time(&timestamp);
        time_info = localtime(&timestamp);
        strftime(time_buffer, DISPLAY_COL_NUM + 1, "%Y-%02m-%02d  %02H:%02M:%02S", time_info);

        pthread_mutex_lock(&display_mutex);
        display_string_row(time_buffer, DISPLAY_ROW_BOTTOM);
        pthread_mutex_unlock(&display_mutex);

        usleep(500 * 1000);
    }

    return 0;
}

int main(int argc, char** argv) {
    gpio_t gpio = gpio_init();

    if(gpio == NULL) {
        printf("Failed to initialize GPIO! Errno: %d\n", errno);
        return -1;
    }

    HD44780_config_t lcd_config = {
		.D4 = GPIO_PIN_12,
		.D5 = GPIO_PIN_15,
		.D6 = GPIO_PIN_16,
		.D7 = GPIO_PIN_18,
		.RS = GPIO_PIN_7,
		.E = GPIO_PIN_11,
		.type = HD44780_DISPLAY_20x2,
		.entry_mode_flags = HD44780_INCREASE_CURSOR_ON,
		.on_off_flags = HD44780_DISPLAY_ON
    };

    display_init(gpio, &lcd_config);

    int error = 0;
    buffer_t raw_buffer, parsed_buffer;

    error = buffer_init(&raw_buffer);
    if(error) {
        printf("Not enough memory to allocate raw data buffer!\n");

        /* Release resources */
        gpio_deinit();
        return -2;
    }

    error = buffer_init(&parsed_buffer);
    if(error) {
        printf("Not enough memory to allocate parsed data buffer!\n");
        
        /* Release resources */
        buffer_deinit(&raw_buffer);
        gpio_deinit();
        return -3;
    }

    error = pthread_mutex_init(&display_mutex, NULL);
    if(error) {
        printf("Failed to initalize display mutex!\n");

        /* Release resources */
        buffer_deinit(&raw_buffer);
        buffer_deinit(&parsed_buffer);
        gpio_deinit();
    }

    pthread_t time_thread;
    error = pthread_create(&time_thread, NULL, time_task, NULL);
    if(error) {
        printf("Failed to spawn time update thread!\n");

        /* Release resources */
        buffer_deinit(&raw_buffer);
        buffer_deinit(&parsed_buffer);
        gpio_deinit();
        return -4;
    }

    /* Capture SIGINT (Ctrl^C) signal to gracefully terminate */
    struct sigaction sigint_config = {0};
    sigint_config.sa_handler = sigint_handler;
    sigaction(SIGINT, &sigint_config, NULL);

    printf("Ticker started!\n");

    while(running) {
        /* Reset buffers state */
        buffer_reset(&raw_buffer);
        buffer_reset(&parsed_buffer);

        /* Fetch new data */
        error = fetcher_fetch(&raw_buffer);
        if(error) {
            printf("Failed to fetch data! Retrying...\n");
            usleep(1 * 1000 * 1000); // Retry after 1 second
        }

        /* Parse data */
        parser_parse(&raw_buffer, &parsed_buffer); // TODO error handling, as there's A LOT of things that might fail in parser...

        /* Substitute polish diacritic codes for HD44780 custom char codes */
        display_substitute_diacritics(parsed_buffer.data);

        /* Display char by char to form moving ticker */
        const char* const ticker_string = parsed_buffer.data;
        size_t ticker_length = strlen(ticker_string);
        for(size_t i = 0; running && i < ticker_length; i++) {
            pthread_mutex_lock(&display_mutex);
            display_update_row(ticker_string[i], DISPLAY_ROW_TOP);
            pthread_mutex_unlock(&display_mutex);

            usleep(TICKER_REFRESH_DELAY_MS * 1000);
        }
    }

    /* Wait for time thread to terminate */
    pthread_join(time_thread, NULL);

    /* Release resources */
    buffer_deinit(&raw_buffer);
    buffer_deinit(&parsed_buffer);
    gpio_deinit();

    return 0;
}
