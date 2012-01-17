#include <pthread.h>
#include <unistd.h>
#include <my_global.h>

#include "mysql_async.h"

/* Runs a query, which sleeps for 10 seconds, and
   in the main thread parallelly also sleeps for 10 seconds. */
int main(int argc, char* argv[])
{
    int retval;
    // Points to thread descriptor
    pthread_t *sqlthread;
    // For passing parameters to new thread's sqlFunction
    SQLThreadParams *params = (SQLThreadParams *)malloc(sizeof(SQLThreadParams));
    
    // Setting parameters
    params->conn = getConnection();
    params->query = "SELECT SLEEP(10)";
    
    // Creating and starting new thread
    printf("Creating and starting SQL thread\n");
    sqlthread = createBackgroundQuery(params);
    
    // Do some important stuff...
    printf("Doing some important stuff\n");
    sleep(5);
    if (isThreadRunning(sqlthread)) {
        printf("Background query is still running\n");
    } else {
        printf("Background query finished\n");
    }
    sleep(5);
    if (isThreadRunning(sqlthread)) {
        printf("Background query is still running\n");
    } else {
        printf("Background query finished\n");
    }
    printf("Important stuff finished\n");
    
    // Wait for the sqlthread finish
    retval = finishBackgroundQuery(sqlthread);
    printf("Background query finished, return value: %d\n", retval);
    free(params);
    
    return 0;
}

/* Connects to MySQL server
   Returns a connection descriptor */
MYSQL* getConnection()
{
    MYSQL *conn;

    if ((conn = mysql_init(NULL)) == NULL) {
        handle_mysql_error(conn);
    }
    
    if (mysql_real_connect(conn, DBHOST, DBUSER, DBPASS, DB, DBPORT, NULL, 0) == NULL) {
        handle_mysql_error(conn);
    }
    
    return conn;
}

/* Creates a thread, and start the sqlFunction() in it with params parameter
   Returns with the thread descriptor */
pthread_t* createBackgroundQuery(SQLThreadParams *params)
{
    pthread_t *sqlthread;
    int s;
    
    sqlthread = (pthread_t *)malloc(sizeof(pthread_t));
    
    // Creating and starting the thread
    if ((s = pthread_create(sqlthread, NULL, sqlFunction, (void *) params)) != 0) {
        handle_error_en(s, "pthread_create");
    }
    
    return sqlthread;
}

/* Checks whether the thread given by sqlthread parameter is running, or
   finished */
int isThreadRunning(pthread_t* sqlthread)
{
    // Sending test signal to thread
    if (pthread_kill(*sqlthread, 0) == ESRCH) {
        return FALSE;
    } else {
        return TRUE;
    }
}

/* Waits for the thread given by sqlthread parameter to finish, then destructs
   the thread descriptor structure
   Returns with the return value of sqlFunction() */
int finishBackgroundQuery(pthread_t* sqlthread)
{
    int s;
    void *retvalptr;
    
    // Waiting for thread to finish
    if ((s = pthread_join(*sqlthread, &retvalptr)) != 0) {
        handle_error_en(s, "pthread_join");
    }
    
    // Destruct threadthe descriptor
    free(sqlthread);
    
    // Returns the pointed value of retvalptr as an int
    return *(int *)retvalptr;
}

/* Runs in a thread, gets parameters in a SQLThreadParams structure,  makes a
   query to db
   Returns the retcode of the query */
void* sqlFunction(void *paramsv)
{
    // Casting back to SQLThreadParams
    SQLThreadParams *params = (SQLThreadParams *)paramsv;
    int *retval;
    
    retval = (int *)malloc(sizeof(int));
    // Making the query, getting the retval
    *retval = mysql_query(params->conn, params->query);
    
    return retval;
}

