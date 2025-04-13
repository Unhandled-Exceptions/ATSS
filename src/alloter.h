#ifndef ALLOTER_H

#define ALLOTER_H
#define INIT_alloted_SIZE 7

#include <stddef.h>
#include <sqlite3.h>

typedef struct alloted_data{
    char alloted_id[10];
    char flight_id[10];
    char allotted_time[5];
} AD;

void view_allots(sqlite3 *db, char *err_msg);
int view_allots_cb(void *NotUsed, int argc, char **argv, char **azColName);
void add_allot();
void get_last_alloted_flight();
void remove_last_alloted_flight();

#endif