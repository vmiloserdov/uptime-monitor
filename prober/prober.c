#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <curl/curl.h>
#include <libpq-fe.h>
#include <pthread.h>


/* Global Variables */
const long timeout = 10L;
const long follow_location = 1L;
const long download_body = 1L;
const char* conn_string ="host=localhost user=monitor_user password=monitor_user dbname=uptime_monitor";



typedef struct {
    int monitor_id;
    char url[512];
} MonitorTask;


/**
 * @brief Function establishes a connection with the local database
 * @return PGconn object if successfully connected, NULL o/w
*/
PGconn *connect_db(){
    PGconn *conn = PQconnectdb(conn_string);
    if (PQstatus(conn) != CONNECTION_OK){
        printf("connection failed: %s\n", PQerrorMessage(conn));
        return NULL;
    }
    return conn;
}



void check_url(PGconn *conn, int monitor_id, char *url){
    CURL *curl;
    CURLcode result;
    long status_code = 0;
    double response_time = 0;

    curl = curl_easy_init();
    if (!curl){
        printf("Failed to initiate curl\n");
        return;
    }

    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, timeout);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, follow_location);
    curl_easy_setopt(curl, CURLOPT_NOBODY, download_body);

    result = curl_easy_perform(curl);

    int is_up = 0;
    char error_msg[256] = "";

    if (result != CURLE_OK){
        // -1 in the end allows the null terminator to be added, for safe behaviour
        strncpy(error_msg, curl_easy_strerror(result), sizeof(error_msg) - 1);
    }
    else{
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &status_code);
        curl_easy_getinfo(curl, CURLINFO_TOTAL_TIME, &response_time);
        is_up = status_code >= 200 && status_code < 400;
    }

    curl_easy_cleanup(curl);

    int response_time_ms = (int)(response_time * 1000);
    char monitor_id_str[16];
    char status_code_str[16];
    char response_time_str[16];
    char is_up_str[8];

    // Safely print the result
    snprintf(monitor_id_str, sizeof(monitor_id_str), "%d", monitor_id);
    snprintf(status_code_str, sizeof(status_code_str), "%ld", status_code);
    snprintf(response_time_str, sizeof(response_time_str), "%d", response_time_ms);
    strncpy(is_up_str, is_up ? "true" : "false", sizeof(is_up_str));


    const char* params[5];
    params[0] = monitor_id_str;
    params[1] = status_code_str;
    params[2] = response_time_str;
    params[3] = is_up_str;
    params[4] = strlen(error_msg) > 0 ? error_msg : NULL;


    PGresult* res = PQexecParams(
        conn,
        "INSERT INTO checks (monitor_id, status_code, response_time_ms, is_up, error_message) VALUES ($1, $2, $3, $4, $5)",
        5, NULL, params, NULL, NULL, 0
    );

    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        printf("Insert failed for %s: %s\n", url, PQerrorMessage(conn));
    } else {
        printf("%s | %s | %ldms | %s\n", url, is_up ? "UP" : "DOWN", status_code, response_time_str);
    }

    PQclear(res);
}

void* thread_check(void* arg) {
    MonitorTask* task = (MonitorTask*)arg;
    
    PGconn* conn = connect_db();
    if (!conn) {
        free(task);
        return NULL;
    }

    check_url(conn, task->monitor_id, task->url);
    
    PQfinish(conn);
    free(task);
    return NULL;
}

int main() {
    PGconn* conn = connect_db();
    if (!conn) return 1;

    while (1) {
        PGresult* res = PQexec(conn, "SELECT id, url FROM monitors WHERE is_active = true");

        if (PQresultStatus(res) != PGRES_TUPLES_OK) {
            printf("Query failed: %s\n", PQerrorMessage(conn));
            PQclear(res);
            break;
        }

        int rows = PQntuples(res);
        printf("Checking %d monitors...\n", rows);

        pthread_t threads[rows];

        for (int i = 0; i < rows; i++) {
            MonitorTask* task = malloc(sizeof(MonitorTask));
            task->monitor_id = atoi(PQgetvalue(res, i, 0));
            strncpy(task->url, PQgetvalue(res, i, 1), sizeof(task->url) - 1);
            pthread_create(&threads[i], NULL, thread_check, task);
        }

        for (int i = 0; i < rows; i++) {
            pthread_join(threads[i], NULL);
        }

        PQclear(res);
        printf("Done. Sleeping 30 seconds...\n\n");
        sleep(30);
    }

    PQfinish(conn);
    return 0;
}