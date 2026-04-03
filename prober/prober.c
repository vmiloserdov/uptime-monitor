#include <stdio.h>
#include <curl/curl.h>

/* Global Variables */
const long timeout = 10L;
const long follow_location = 1L;
const long download_body = 1L;


int main(int argc, char **argv){
    if (argc < 2){
        printf("Wrong usage!\n");
        return 1;
    }

    char *url = argv[1];
    CURL *curl;
    CURLcode result;
    long status_code;
    double response_time;

    // Allocate and return an easy handle
    curl = curl_easy_init();
    if (!curl){
        printf("curl_easy_init() error!\n");
        return 1;
    }

    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, timeout);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, follow_location);
    curl_easy_setopt(curl, CURLOPT_NOBODY, download_body);

    result = curl_easy_perform(curl);
    if (result != CURLE_OK){
        printf("DOWN | Error : %s\n", curl_easy_strerror(result));
        curl_easy_cleanup(curl);
        return 0;
    }

    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &status_code);
    curl_easy_getinfo(curl, CURLINFO_TOTAL_TIME, &response_time);

    int is_up = status_code >= 200 && status_code < 400;
    printf("%s | %ld |  %.0fms\n", is_up ? "UP" : "DOWN", status_code, response_time * 1000);

    curl_easy_cleanup(curl);
    return 0;



}