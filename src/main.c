/* Created by Lefucjusz, 01.09.2022, Gdańsk */
#include "GPIO.h"
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <curl/curl.h>
#include <string.h>
#include <stdbool.h>
#include "buffer.h"

//TODO add const modifiers to pointers

#define DATE_LENGTH 10 // YYYY-MM-DD
#define TIME_LENGTH 8 // HH:MM:SS

buffer_t buffer;

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

static size_t get_tag_content(char* stream, const char* const tag, char** content) {
    /* Search for the tag */
    char* start = strstr(stream, tag);
    
    /* If tag not found - set content to NULL and return zero length */
    if(start == NULL) {
        *content == NULL;
        return 0;
    } 
    
    /* Obtain position of the beginning of the content - skip tag */
    start += strlen(tag);

    /* Check if content in CDATA */
    const char* const cdata_str = "<![CDATA[";
    bool is_cdata = false;
    if(strstr(start, cdata_str) == start) {
        is_cdata = true;
        start += strlen(cdata_str);
    }

    /* Obtain position of the end of the content */
    char* end = NULL;
    if(is_cdata) {
        end = strchr(start, ']') - 1; // Look for end of CDATA and move to previous char
    } else {
        end = strchr(start, '<') - 1; // Look for the ending tag and move to previous char
    }

    /* Set content pointer to proper value */
    *content = start;

    /* Return length */
    return end - start + 1;
}

void parse_news_xml(char* stream) { //TODO proper type
    char* content = NULL;
    size_t length = 0;

    /* Prepare main title */
    length = get_tag_content(stream, "<title>", &content); //TODO check buffer_append retcodes
    buffer_append(&buffer, "#", 0);
    buffer_append(&buffer, content, length);
    buffer_append(&buffer, "# ostatnia aktualizacja: ", 0);

    /* Prepare date */
    get_tag_content(content, "<lastBuildDate>", &content);
    /* Date is in YYYY-MM-DDTHH:MM:SS.000Z format, parse to get YYYY-MM-DD HH:MM:SS format */
    buffer_append(&buffer, content, DATE_LENGTH);
    buffer_append(&buffer, " ", 0);
    buffer_append(&buffer, content + DATE_LENGTH + 1, TIME_LENGTH); // +1 to skip 'T'

    /* Prepare all items - iterate until no more <item> tags left */
    while((length = get_tag_content(content, "<item>", &content)) != 0) {
        length = get_tag_content(content, "<title>", &content);
        debug_print_array(content, length);
    };    
}

static const char* const news_url = "https://tvn24.pl/najnowsze.xml";
static const char* user_agent = "Mozilla/5.0";

static size_t curl_write_cb(void* data, size_t size, size_t nmemb, void* userp) {
    size_t real_size = size * nmemb;

    parse_news_xml((char*)data);

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
    // volatile unsigned int* const gpio = gpio_init();

    // if(gpio == NULL) {
    //     printf("Failed to initialize GPIO! Errno: %d\n", errno);
    //     return -1;
    // }
    
    buffer_init(&buffer);

    int ret = curl_test();
    printf("Retcode: %d\n", ret);

    // debug_print_array(buffer.string, strlen(buffer.string)+1);

    buffer_deinit(&buffer);

    // int ret = curl_test();

    // printf("Retcode: %d\n", ret);

    // if(ret == 0) {
    //    printf("Apparently it works!\n");
    // }

    // gpio_deinit();
    return 0;
}
