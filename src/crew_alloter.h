// crew_alloter.h
#ifndef CREW_ALLOTER_H
#define CREW_ALLOTER_H

#include <sqlite3.h>
#include "flights.h"
#include "crew.h"
#include "alloter.h" 

#define REQUIRED_PILOTS 2
#define REQUIRED_ATTENDANTS 3
#define MAX_HOURS_WORKED 60

typedef struct {
    char flight_id[10];
    int crew_id;
    char designation[50];
} CrewAssignment;

float calculate_flight_duration_hours(const char *departure_time, const char *arrival_time);
int create_crew_allotment_table(sqlite3 *db, char **err_msg);
int allot_crew_for_flights(FL *flights, CL *crew_list, sqlite3 *db, char **err_msg);
void view_crew_allotments(sqlite3 *db, char **err_msg);
int view_crew_allotments_cb(void *NotUsed, int argc, char **argv, char **azColName);
int update_batch_crew_hours_in_db(CL *crew_list, sqlite3 *db, char **err_msg);


#endif