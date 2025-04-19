#include "alloter.h"
#include "flights.h"
#include <sqlite3.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
Use
pragma foreign_keys = on
when inserting to enable foreign key constraint.
sqlite does not do it by default.
*/

/*
Prints allotment table to console.
*/
void view_allots(sqlite3 *db, char *err_msg)
{
    // printf("Inside view allot fun\n");
    char *query = "SELECT * FROM alloted ORDER BY allotted_time;";

    printf("ID\tFLIGHT ID\tTIME\n");
    int rc = sqlite3_exec(db, query, view_allots_cb, 0, &err_msg);

    if (rc != SQLITE_OK) {

        fprintf(stderr, "Failed to select data\n");
        fprintf(stderr, "SQL error: %s\n", err_msg);

        sqlite3_free(err_msg);
        sqlite3_close(db);

        return;
    }
    return;
}

int view_allots_cb(void *NotUsed, int argc, char **argv, char **azColName)
{
    NotUsed = 0;

    printf("%s\t%s\t\t%s\n",
           argv[0] ? argv[0] : "NULL",
           argv[1] ? argv[1] : "NULL",
           argv[2] ? argv[2] : "NULL");
    return 0;
}

/*
Returns an FD struct (values, not pointer) of the last allotted flight.
*/
AD get_last_alloted_flight(FL *flights, sqlite3 *db, char *err_msg)
{
    // printf("Inside view allot fun\n");

    AD last_allot;
    char *query = "SELECT * FROM alloted ORDER BY allotted_time DESC LIMIT 1;";

    // printf("ID\tFLIGHT ID\tTIME\n");
    int rc = sqlite3_exec(db, query, get_last_alloted_flight_cb, &last_allot, &err_msg);
    // printf("got it out %s\n", last_allot.flight_id);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to select data\n");
        fprintf(stderr, "SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
        sqlite3_close(db);
        return (AD){0}; // Did you know you could return empty structs!?
    }

    // Linear search through the flight array
    for (int i = 0; i < flights->size; i++) {
        if (strcmp(flights->flight[i].flight_id, last_allot.flight_id)) {
            // return flights->flight[i];
            return last_allot;
        }
    }

    return (AD){0};
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
*/
void add_allot(char *flight_id, char *alloted_time, sqlite3 *db, char *err_msg)
{

    char *query_template = "INSERT INTO alloted (flight_id, allotted_time) VALUES ('%q', '%q');";
    char *query = sqlite3_mprintf(query_template, flight_id, alloted_time);

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
    }
}

/*
Allotment func - Implemented using algo given in README
current code is very skeletony.
Work on making `diff` proper and better logic.
*/
void allotment(FL *flights, sqlite3 *db, char *err_msg)
{
    // First one needs no check.
    add_allot(flights->flight[0].flight_id, flights->flight[0].runway_time,
        db, err_msg);    

    // Iterating through flight table
    for (int i = 1; i < flights->size; i++) {
        AD prevAllot = get_last_alloted_flight(flights, db, err_msg);

        int diff = prevAllot.allotted_time - flights->flight[i].runway_time;
        if (diff < 15) {
            printf("Conflicting...\n");
        }
        else {
            printf("No Conflict\n");
            add_allot(flights->flight[i].flight_id, flights->flight[i].runway_time,
                      db, err_msg);
        }
    }
}
