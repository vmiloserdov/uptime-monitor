#include <stdio.h>
#include <libpq-fe.h>

char *conn_string = "host=localhost user=monitor_user password=monitor_user dbname=uptime_monitor";

int main(){

    PGconn *conn = PQconnectdb(conn_string);
    if (PQstatus(conn) != CONNECTION_OK){
        printf("Connection failed: %s\n", PQerrorMessage(conn));
        PQfinish(conn);
        return 1;
    }

    printf("Successfully connected!\n");

    PGresult *res = PQexecParams(
        conn, 
        "INSERT INTO checks (monitor_id, status_code, response_time_ms, is_up) VALUES ($1, $2, $3, $4)",
        4,
        NULL, 
        (const char*[]){"1", "200", "143", "true"},
        NULL,
        NULL, 
        0
    );

    if (PQresultStatus(res) != PGRES_COMMAND_OK){
        printf("Insert failed: %s\n", PQerrorMessage(conn));
    }
    else{
        printf("Insert succeeded!\n");
    }

    PQclear(res);
    PQfinish(conn);
    return 0;
}