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
    clear_screen();
    char *query = "SELECT * FROM alloted ORDER BY allotted_time;";

    clear_screen();
    printf("=========================================\n");
    printf("            Flight Allotments            \n");
    printf("=========================================\n\n");
    printf("-----------------------------------------\n");
    printf("%-6s %-12s %-8s %s\n", "ID", "Flight ID", "Time", "Runway");
    printf("-----------------------------------------\n");

    int rc = sqlite3_exec(db, query, view_allots_cb, 0, &err_msg);
    printf("-------------------------------------------------\n");

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

    printf("%-6s %-12s %-8s %s\n",
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

    AD last_allot = {0}; // This thing must be initialised as 0.
    AD invalid_allot = {0};
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

    // DEBUG TO FIX RUNWAY 3 BEING AVOIDED.
    // if (runway == 3)
    // {
    //     printf("\e[1m R3 Special Case: %s\n\e[0m", last_allot.flight_id);
    // }

    // Linear search through the flight array
    for (int i = 0; i < flights->size; i++) {
        if (strcmp(flights->flight[i].flight_id, last_allot.flight_id) == 0) {
            // printf("It's a valid flight with ID %s\n", flights->flight[i].flight_id);
            // return flights->flight[i];
            return last_allot;
        }
    }

    printf("  There are no flights in this runway\n");
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

/*Adds a flight to the allot table.
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

/*
Removes the last instance of given flight ID.
*/
void remove_allot(char *flight_id, int runway, sqlite3 *db, char *err_msg)
{
    // ORDER BY ERROR IN SQLITE
    // char *query_template = "DELETE FROM alloted WHERE runway=%d AND flight_id=%s ORDER BY allotted_time DESC LIMIT 1;";
    char *query_template = "DELETE FROM alloted WHERE flight_id='%s';";
    char *query = sqlite3_mprintf(query_template, flight_id);

    if (query == NULL) {
        printf("Memory allocation for the query failed :(\n");
    }

    int rc = sqlite3_exec(db, query, 0, 0, &err_msg);

    if (rc != SQLITE_OK) {
        printf("remove_allot(): SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
        sqlite3_free(query);
        return;
    }
    else {
        printf("Removed flight %s!\n", flight_id);
        return;
    }
}

int time_in_minutes(const char *time)
{
    if (time == NULL || strlen(time) != 4) {
        return -1; // Indicate invalid input
    }
    int hour1 = (time[0] - '0') * 10 + (time[1] - '0');
    int minute1 = (time[2] - '0') * 10 + (time[3] - '0');

    if (hour1 < 0 || hour1 > 23 || minute1 < 0 || minute1 > 59) {
        printf("\tInvalid time format.\n");
        return -1; // Indicate invalid time format
    }

    int totalMinutes1 = hour1 * 60 + minute1;

    return totalMinutes1;
}

int timedifference(const char *time1, const char *time2)
{
    int totalMinutes1 = time_in_minutes(time1);
    int totalMinutes2 = time_in_minutes(time2);

    return abs(totalMinutes1 - totalMinutes2);
}

/* Allotment func - Implemented using algo given in README
As of now, I'm making it so that each time this function is called
it deletes the prexisting table
*/
struct delaydata allotment(FL *flights, sqlite3 *db, char *err_msg)
{
    // Declaration of variables
    printf("=========================================\n");
    printf("          Starting Flight Allotment       \n");
    printf("=========================================\n\n");
    printf("-------------------------------------------------\n");
    char delaypile[50][10];
    int delaycount = 0;

    // First reset the table to empty state
    char *query = "DELETE FROM alloted;";
    // For some reason resetting the autoincrement counter results in a segfault.
    // char *query = "DELETE FROM alloted; DELETE FROM SQLITE_SEQUENCE WHERE name='alloted';";
    int rc = sqlite3_exec(db, query, 0, 0, &err_msg);
    if (rc != SQLITE_OK) {
        printf("SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
        sqlite3_free(query);
        return (struct delaydata){0};
    }

    // First one needs no check. To runway 1
    add_allot(flights->flight[0].flight_id, flights->flight[0].runway_time, 1,
              db, err_msg);

    // Iterating through flight table
    for (int i = 1; i < flights->size; i++) {
        AD prevAllot = get_last_alloted_flight(flights, 1, db, err_msg);

        int diff = timedifference(prevAllot.allotted_time, flights->flight[i].runway_time);
        printf("\n------------------------------------------------------\n");
        printf("FID: %s\tPREV TIME: %s\tCUR TIME: %s\tDIFF: %d\n", flights->flight[i].flight_id, prevAllot.allotted_time, flights->flight[i].runway_time, diff);

        if (diff < 15) {
            printf("\e[33mConflicting...\e[0m\n");
            int resolved_flag = 0;
            for (int r = 2; r <= RUNWAYCOUNT; r++) {
                printf("  Changing runway to %d\n", r);

                AD prevAllot = get_last_alloted_flight(flights, r, db, err_msg);
                // printf("  Last alloted flight in runway %d is %d\n", r, prevAllot.allot_id);

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

            // Priority switchup.
            if (resolved_flag == 0) {
                // Looping through runways once again with the intention to remove lower priority flights.
                for (int r = 1; r <= RUNWAYCOUNT; r++) {
                    printf("  Changing runway to %d\n", r);

                    AD prevAllot = get_last_alloted_flight(flights, r, db, err_msg);
                    // printf("  Last alloted flight in runway %d is %d\n", r, prevAllot.allot_id);

                    int cur_priority = flights->flight[i].priority_level;
                    int prev_priority = find_flight_by_id(prevAllot.flight_id, flights)->priority_level;
                    if (cur_priority < prev_priority) {
                        printf("\e[1;36m  Higher priority flight, putting flight %s in delay pile\n\e[0m", prevAllot.flight_id);
                        remove_allot(prevAllot.flight_id, prevAllot.runway, db, err_msg);

                        strcpy(delaypile[delaycount], prevAllot.flight_id);
                        delaycount++;

                        add_allot(flights->flight[i].flight_id, flights->flight[i].runway_time, r,
                                  db, err_msg);
                        resolved_flag = 1;
                        break;
                    }
                }
            }

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

    // returning delayed data
    struct delaydata rtrndata;
    rtrndata.delaycount = delaycount;

    // linearly copying everything
    for (int i = 0; i < delaycount; i++) {
        strcpy(rtrndata.delaypile[i], delaypile[i]);
    }

    return rtrndata;
}

void tui_allotment(FL *flights, sqlite3 *db, char *err_msg)
{
    clear_screen();

    struct delaydata delayd = allotment(flights, db, err_msg);

    int delaycount = delayd.delaycount;

    if (delaycount == 0) {
        printf("Everything is properly alloted.\n");
    }
    else {
        printf("\e[33;1;4mConflicting Flights\e[0m\n");
        printf("c to cancel, d to delay\n");
        for (int i = 0; i < delaycount; i++) {
            char op;
            printf("%s: ", delayd.delaypile[i]);
            fflush(stdin);
            scanf("%c", &op);
            if (op == 'c') {
                printf("Flight Cancelled!\n");
                fflush(stdout);
            }
            else if (op == 'd') {
                printf("New Timing: ");
                char newtime[5];
                scanf(" %s", newtime);
                add_allot(flights->flight[i].flight_id, newtime, 1, db, err_msg);
                printf("Flight delayed to %s\n", newtime);
                fflush(stdout);
            }
            fflush(stdin);
        }
        printf("Conflicts resolved!");
        scanf("%c"); // Absorber of \n;
        fflush(stdout);
        fflush(stdin);
    }

    pauseScreen();
}

void utilization_report(FL *flights, sqlite3 *db, char *err_msg)
{
    clear_screen();
    printf("=========================================\n");
    printf("            Runway Utilization           \n");
    printf("=========================================\n\n");

    for (int r = 1; r <= RUNWAYCOUNT; r++) {
        // int util96[96]; // 24 hrs * 4 quarters of 15 mins, for data viz
        int util_time = 0;
        struct utildata utilvars;

        utilvars.util96 = (int *)calloc(96, sizeof(int));
        utilvars.util_time = &util_time;

        // [================================================================================================]
        char *query_template = "SELECT * FROM alloted WHERE runway=%d ORDER BY allotted_time;";
        char *query = sqlite3_mprintf(query_template, r);

        int rc = sqlite3_exec(db, query, utilization_report_cb, &utilvars, &err_msg);

        int util_perc = (util_time * 100) / 1440;
        printf("Runway #%d is used for %d minutes -> %d%% Usage.\n", r, util_time, util_perc);
        printf("[");
        for (int i = 0; i < 96; i++) {
            if (utilvars.util96[i] == 1) {
                printf("\e[32m█\e[0m");
            }
            else {
                printf("\e[2m■\e[0m");
            }
        }
        printf("]\n\n");
        free(utilvars.util96);
    }

    pauseScreen();
}

int utilization_report_cb(void *utilvars, int argc, char **argv, char **azColName)
{
    struct utildata *uv = (struct utildata *)utilvars;

    *uv->util_time += 15;
    // printf("utilzd time is %d\n", *uv->util_time);

    char alot_time[5];
    strcpy(alot_time, argv[2]);

    int s_inteval = 0;
    int end_interval = 14;

    int flight_time = time_in_minutes(alot_time);

    for (int i = 0; i < 96; i++) {
        if (flight_time > s_inteval && flight_time < end_interval) {
            uv->util96[i] = 1;
            // printf("time is %s (ie %d mins)\n", alot_time, flight_time);
            return 0;
        }
        s_inteval += 15;
        end_interval += 15;
    }
    return 0;
}
