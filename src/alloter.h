#ifndef ALLOTER_H

#define ALLOTER_H
#define INIT_alloted_SIZE 7

#include "flights.h"
#include "utils.h"
#include <math.h>
#include <sqlite3.h>
#include <stddef.h>

typedef struct alloted_data {
    int allot_id;
    char flight_id[10];
    int runway;
    char allotted_time[5];
} AD;

struct utildata {
    int *util96;
    int *util_time;
};

#define RUNWAYCOUNT 3

void view_allots(sqlite3 *db, char *err_msg);
int view_allots_cb(void *NotUsed, int argc, char **argv, char **azColName);

void allotment(FL *flights, sqlite3 *db, char *err_msg);
void add_allot(char *flight_id, char *alloted_time, int runway, sqlite3 *db, char *err_msg);
void remove_allot(char *flight_id, int runway, sqlite3 *db, char *err_msg);

int time_in_minutes(const char *time1);
int timedifference(const char *time1, const char *time2);

AD get_last_alloted_flight(FL *flights, int runway, sqlite3 *db, char *err_msg);
int get_last_alloted_flight_cb(void *la, int argc, char **argv, char **azColName);

void utilization_report(FL *flights, sqlite3 *db, char *err_msg);
int utilization_report_cb(void *utilvars, int argc, char **argv, char **azColName);
#endif