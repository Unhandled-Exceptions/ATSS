#include "alloter.h"
#include "flights.h"
#include "utils.h"
#include <math.h>
#include <sqlite3.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
Prints allotment table to console.
Somebody please make it look good and match with modern formatting.
*/
void view_allots(sqlite3 *db, char *err_msg)
{
    // printf("Inside view allot fun\n");
    char *query = "SELECT * FROM alloted ORDER BY allotted_time;";

    printf("ID\tFLIGHT ID\tTIME\tRUNWAY\n");
    int rc = sqlite3_exec(db, query, view_allots_cb, 0, &err_msg);

    if (rc != SQLITE_OK) {

        fprintf(stderr, "Failed to select data\n");
        fprintf(stderr, "SQL error: %s\n", err_msg);

        sqlite3_free(err_msg);
        sqlite3_close(db);

        return;
    }

    pauseScreen();
    return;
}

int view_allots_cb(void *NotUsed, int argc, char **argv, char **azColName)
{
    NotUsed = 0;

    printf("%s\t %s\t %s\t %s\n",
           argv[0] ? argv[0] : "NULL",
           argv[1] ? argv[1] : "NULL",
           argv[2] ? argv[2] : "NULL",
           argv[3] ? argv[3] : "NULL");
    return 0;
}

/*
Returns an FD struct (values, not pointer) of the last allotted flight.
*/
AD get_last_alloted_flight(FL *flights, int runway, sqlite3 *db, char *err_msg)
{
    // printf("Inside view allot fun\n");

    AD last_allot;
    AD invalid_allot;
    invalid_allot.allot_id = -1;

    char *query_template = "SELECT * FROM alloted WHERE runway=%d ORDER BY allotted_time DESC LIMIT 1;";
    char *query = sqlite3_mprintf(query_template, runway);

    // printf("ID\tFLIGHT ID\tTIME\n");
    int rc = sqlite3_exec(db, query, get_last_alloted_flight_cb, &last_allot, &err_msg);
    // printf("got it out %s\n", last_allot.flight_id);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to select data\n");
        fprintf(stderr, "SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
        sqlite3_close(db);
        // return (AD){0}; // Did you know you could return empty structs!?
        return invalid_allot;
    }

    // Linear search through the flight array
    for (int i = 0; i < flights->size; i++) {
        if (strcmp(flights->flight[i].flight_id, last_allot.flight_id) == 0) {
            // printf("It's a valid flight with ID %s\n", flights->flight[i].flight_id);
            // return flights->flight[i];
            return last_allot;
        }
    }

    printf("There are no flights in this runway\n");
    return invalid_allot;
}

// Internal CB for sqlite
int get_last_alloted_flight_cb(void *la, int argc, char **argv, char **azColName)
{

    AD *last_allot = (AD *)la;

    // printf("%s\t%s\t\t%s\n",
    //        argv[0] ? argv[0] : "NULL",
    //        argv[1] ? argv[1] : "NULL",
    //        argv[2] ? argv[2] : "NULL");

    last_allot->allot_id = atoi(argv[0]);
    // strcpy(last_allot->allot_id, atoi(argv[0]));
    strcpy(last_allot->flight_id, argv[1]);
    strcpy(last_allot->allotted_time, argv[2]);

    // printf("got it in the thing %s\n", last_allot->flight_id);

    return 0;
}

void remove_last_alloted_flight();

/*
Adds a flight to the allot table.

TODO:
Use
pragma foreign_keys = on
when inserting to enable foreign key constraint.
sqlite does not do it by default.

*/
void add_allot(char *flight_id, char *alloted_time, int runway, sqlite3 *db, char *err_msg)
{

    char *query_template = "INSERT INTO alloted (flight_id, allotted_time, runway) VALUES ('%q', '%q', %d);";
    char *query = sqlite3_mprintf(query_template, flight_id, alloted_time, runway);

    if (query == NULL) {
        printf("Memory allocation for the query failed :(\n");
    }

    int rc = sqlite3_exec(db, query, 0, 0, &err_msg);

    if (rc != SQLITE_OK) {
        printf("SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
        sqlite3_free(query);
        return;
    }
    else {
        printf("Alloted flight %s!\n", flight_id);
        return;
    }
}

int timedifference(const char *time1, const char *time2)
{
    if (time1 == NULL || time2 == NULL || strlen(time1) != 4 || strlen(time2) != 4) {
        return -1; // Indicate invalid input
    }

    int hour1 = (time1[0] - '0') * 10 + (time1[1] - '0');
    int minute1 = (time1[2] - '0') * 10 + (time1[3] - '0');
    int hour2 = (time2[0] - '0') * 10 + (time2[1] - '0');
    int minute2 = (time2[2] - '0') * 10 + (time2[3] - '0');

    if (hour1 < 0 || hour1 > 23 || minute1 < 0 || minute1 > 59 ||
        hour2 < 0 || hour2 > 23 || minute2 < 0 || minute2 > 59) {
        return -1; // Indicate invalid time format
    }

    int totalMinutes1 = hour1 * 60 + minute1;
    int totalMinutes2 = hour2 * 60 + minute2;

    return abs(totalMinutes1 - totalMinutes2);
}

/*
Allotment func - Implemented using algo given in README
current code is very skeletony.
Work on making `diff` proper and better logic.

As of now, I'm making it so that each time this function is called
it deletes the prexisting table

*/
void allotment(FL *flights, sqlite3 *db, char *err_msg)
{
    // Declaration of variables
    char delaypile[50][10];
    int delaycount = 0;

    // First reset the table to empty state
    char *query = "DELETE FROM alloted;";
    // For some reason resetting the autoincrement counter results in a segfault.
    // char *query = "DELETE FROM alloted; DELETE FROM SQLITE_SEQUENCE WHERE name='alloted';";
    printf("gonna executin!\n");
    int rc = sqlite3_exec(db, query, 0, 0, &err_msg);
    printf("finish\n");
    if (rc != SQLITE_OK) {
        printf("SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
        sqlite3_free(query);
        return;
    }

    // First one needs no check. To runway 1
    add_allot(flights->flight[0].flight_id, flights->flight[0].runway_time, 1,
              db, err_msg);

    // Iterating through flight table
    for (int i = 1; i < flights->size; i++) {
        AD prevAllot = get_last_alloted_flight(flights, 1, db, err_msg);

        int diff = timedifference(prevAllot.allotted_time, flights->flight[i].runway_time);
        printf("FID: %s\tPREV TIME: %s\tCUR TIME: %s\tDIFF: %d\n", flights->flight[i].flight_id, prevAllot.allotted_time, flights->flight[i].runway_time, diff);

        if (diff < 15) {
            printf("Conflicting...\n");
            int resolved_flag = 0;
            for (int r = 2; r <= 3; r++) {
                printf("  Changing runway to %d\n", r);

                AD prevAllot = get_last_alloted_flight(flights, r, db, err_msg);

                int diff;
                // First time runway alloc
                if (prevAllot.allot_id == -1) {
                    diff = 1000;
                }
                else {
                    diff = timedifference(prevAllot.allotted_time, flights->flight[i].runway_time);
                }

                printf("  PREV TIME: %s\tCUR TIME: %s\tDIFF: %d\n", prevAllot.allotted_time, flights->flight[i].runway_time, diff);
                if (diff > 15) {
                    add_allot(flights->flight[i].flight_id, flights->flight[i].runway_time, r,
                              db, err_msg);
                    resolved_flag = 1;
                    break;
                }
            }

            // Implement the priority switchup here.

            if (resolved_flag == 0) {
                printf("Cannot find runway, moving to delay pile.\n");
                strcpy(delaypile[delaycount], flights->flight[i].flight_id);
                delaycount++;
            }
        }
        else {
            // printf("No Conflict\n");
            add_allot(flights->flight[i].flight_id, flights->flight[i].runway_time, 1,
                      db, err_msg);
        }
    }

    if (delaycount == 0) {
        printf("Everything is properly alloted.\n");
    }
    else {
        printf("Conflicting Flights\n");
        for (int i = 0; i < delaycount; i++) {
            printf("\t%s", delaypile[i]);
        }
    }

    pauseScreen();
}
