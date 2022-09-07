/* Created by Lefucjusz, 01.09.2022, Gdańsk */
#include "GPIO.h"
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <curl/curl.h>
#include <string.h>
#include <stdbool.h>
#include "buffer.h"
#include <ctype.h>
#include "HD44780.h"
#include "parser.h"

buffer_t response_buffer, line_buffer;

static void debug_print_array(char* array, size_t size) {
    for(size_t i = 0; i < size; i++) {
        printf("%c", *(array + i));
    }
    printf(" (ASCII: ");
    for(size_t i = 0; i < size; i++) {
        printf("%d ", (int)*(array + i));
    }
    printf(")\n");
}

//TODO mapDiacritics

static const char* const news_url = "https://tvn24.pl/najnowsze.xml";
static const char* user_agent = "Mozilla/5.0";

static size_t curl_write_cb(void* data, size_t size, size_t nmemb, void* userp) {
    size_t real_size = size * nmemb;

    buffer_append(&response_buffer, data, nmemb);

    printf("Appended %u bytes!\n", real_size);

    return real_size;
}

int curl_test(void) {
    CURL* curl = NULL;
    CURLcode res;

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();
    if(curl == NULL) {
	return -1;
    }

    curl_easy_setopt(curl, CURLOPT_URL, news_url);
    curl_easy_setopt(curl, CURLOPT_USERAGENT, user_agent);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curl_write_cb);

    res = curl_easy_perform(curl);

    if(res != CURLE_OK) {
        printf("Res: %s\n", curl_easy_strerror(res));
    	return -2;
    }

    curl_easy_cleanup(curl);
    curl_global_cleanup();
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

    //HD44780_init(gpio, &lcd_config);

    //HD44780_write_string("Elo!");

    buffer_init(&line_buffer);
    buffer_init(&response_buffer);

    int ret = curl_test();
    printf("Retcode: %d\n", ret);

    parser_parse(&response_buffer, &line_buffer);

    printf("%s\n", line_buffer.data);

    buffer_deinit(&response_buffer);
    buffer_deinit(&line_buffer);

    gpio_deinit();
    return 0;
}
