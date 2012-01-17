#ifndef _MYSQL_ASYNC_H
#define _MYSQL_ASYNC_H

#include <stdio.h>
#include <stdlib.h>
#include <mysql.h>

// DB connect data
#define DBHOST "127.0.0.1"
#define DBUSER "root"
#define DBPASS ""
#define DB "test"
#define DBPORT 0

// Error handler macros
#define handle_error_en(en, msg) \
       do { errno = en; perror(msg); exit(EXIT_FAILURE); } while (0)
#define handle_mysql_error(conn) \
       do { fprintf(stderr, "Error: %u: %s\n", mysql_errno(conn), mysql_error(conn)); exit(EXIT_FAILURE); } while (0)

// Structure for passing parameters to thread function (sqlFunction)
typedef struct {
    MYSQL *conn;
    char *query;
} SQLThreadParams;

pthread_t* createBackgroundQuery(SQLThreadParams *params);
int finishBackgroundQuery(pthread_t* sqlthread);
void *sqlFunction(void *params);
MYSQL* getConnection();
int isThreadRunning(pthread_t* sqlthread);

#endif
