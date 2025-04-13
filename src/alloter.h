#ifndef ALLOTER_H

#define ALLOTER_H
#define INIT_alloted_SIZE 7

#include "flights.h"
#include <sqlite3.h>
#include <stddef.h>

typedef struct alloted_data {
    char alloted_id[10];
    char flight_id[10];
    char allotted_time[5];
} AD;

void view_allots(sqlite3 *db, char *err_msg);
int view_allots_cb(void *NotUsed, int argc, char **argv, char **azColName);

void allotment();
void add_allot();
FD get_last_alloted_flight(FL *flights, sqlite3 *db, char *err_msg);
int get_last_alloted_flight_cb(void *la, int argc, char **argv, char **azColName);

void remove_last_alloted_flight();

#endif