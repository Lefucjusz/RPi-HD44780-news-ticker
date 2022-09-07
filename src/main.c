/* Created by Lefucjusz, 01.09.2022, Gdańsk */
#include "GPIO.h"
#include <stdlib.h>
#include <stdio.h>
#include <curl/curl.h>
#include <string.h>
#include "buffer.h"
#include "HD44780.h"
#include "parser.h"
#include "display.h"
#include <unistd.h>
#include <errno.h>

buffer_t response_buffer, line_buffer;

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

/* Algorithm obtained via printing ASCII codes of diacritics... */
static void remove_diacritics(char* stream) {
    size_t length = strlen(stream);

    for(size_t i = 0; i < length; i++) {
        switch(stream[i]) {
            case 195:
                switch(stream[i + 1]) {
                    case 147: // Ó
                    case 179: // o
                        stream[i + 1] = DISPLAY_POLISH_O;
                        break;
                    default:
                        continue;
                }
                break;
            case 196:
                switch(stream[i + 1]) {
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
                switch(stream[i + 1]) {
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

    buffer_init(&line_buffer);
    buffer_init(&response_buffer);

    while(1) {
        int ret = curl_test();
        printf("Retcode: %d\n", ret);

        parser_parse(&response_buffer, &line_buffer);

        remove_diacritics(line_buffer.data);

        for(size_t i = 0; i < strlen(line_buffer.data); i++) {
            display_update_row(line_buffer.data[i], DISPLAY_ROW_TOP);
            usleep(100 * 1000);
        }
    }    

    buffer_deinit(&response_buffer);
    buffer_deinit(&line_buffer);

    gpio_deinit();
    return 0;
}
