/*
 * fetcher.c
 *
 *  Created on: 08.09.2022
 *      Author: Lefucjusz
 */

#include "fetcher.h"
#include <curl/curl.h>

static const char* const news_url = "https://tvn24.pl/najnowsze.xml";
static const char* const user_agent = "Mozilla/5.0";

static size_t curl_callback(void* data, size_t size, size_t nmemb, void* data_buffer) {
    size_t real_size = size * nmemb;

    /* Append data packet to buffer */
    buffer_append(data_buffer, data, nmemb);

    return real_size;
}

int fetcher_fetch(buffer_t* const fetched_data) {
    CURL* curl_instance = NULL;

    curl_global_init(CURL_GLOBAL_DEFAULT);

    curl_instance = curl_easy_init();
    if(curl_instance == NULL) {
	    return -1;
    }

    /* Set request parameters */
    curl_easy_setopt(curl_instance, CURLOPT_URL, news_url);
    curl_easy_setopt(curl_instance, CURLOPT_USERAGENT, user_agent);
    curl_easy_setopt(curl_instance, CURLOPT_WRITEFUNCTION, curl_callback);
    curl_easy_setopt(curl_instance, CURLOPT_WRITEDATA, fetched_data);

    /* Perform request */
    int ret = curl_easy_perform(curl_instance);

    /* Clean up */
    curl_easy_cleanup(curl_instance);
    curl_global_cleanup();

    return ret;
}