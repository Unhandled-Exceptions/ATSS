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


typedef struct {
    char flight_id[10];
} AllottedFlightInfo;

typedef struct {
    AllottedFlightInfo *flights;
    size_t size;
    size_t allocated;
} AllottedFlightList;

static int get_allotted_flights_cb(void *data, int argc, char **argv, char **azColName);
static void init_allotted_flight_list(AllottedFlightList *list, size_t initial_size);
static void add_to_allotted_flight_list(AllottedFlightList *list, const char *flight_id);
static void free_allotted_flight_list(AllottedFlightList *list);

float calculate_flight_duration_hours(const char *departure_time, const char *arrival_time);
int create_crew_allotment_table(sqlite3 *db, char **err_msg);
int allot_crew_for_flights(FL *flights, CL *crew_list, sqlite3 *db, char **err_msg);
void view_crew_allotments(sqlite3 *db, char **err_msg);
int view_crew_allotments_cb(void *NotUsed, int argc, char **argv, char **azColName);
int update_batch_crew_hours_in_db(CL *crew_list, sqlite3 *db, char **err_msg);
int gui_allot_crew_for_flights(FL *flights, CL *crew_list, sqlite3 *db, int *out_success_count, int *out_fail_count, char **err_msg);


#endif