#ifndef FLIGHTS_H

#define FLIGHTS_H
#define AIRPORT "MAA"
#define INIT_FLIGHTS_SIZE 7

#include <stddef.h>
#include <sqlite3.h>

typedef struct flight_data{
    char flight_id[10];
    char airline [25];
    char origin[4];
    char destination[4];
    char departure_time[5];
    char arrival_time[5];
    char aircraft_type[25];
    int priority_level;
    char runway_time[5];    
} FD;

typedef struct flights_list{
    FD *flight;
    size_t size;
    size_t allocated;
} FL;

extern const char *all_airports[];
extern const char *all_airlines[];
extern const char *all_aircraft_types[];

void init_flight_list(FL *flights, size_t initial_size);
void addto_flight_list(FD *flight_data, FL *flights);
void free_flight_list(FL *flights);
int load_flights_data(FL *flights, sqlite3 *db, char *err_msg);
void load_flights_data_cb(void *fl, int argc, char **argv, char **azColName);
int insert_flight_data(FD *flight, FL *flights, sqlite3 *db, char *err_msg);
int update_flight_data(FD *flight, FL *flights, sqlite3 *db, char *err_msg);
int delete_flight_data(char *flight_id, FL *flights, sqlite3 *db, char *err_msg);
FD* find_flight_by_id(const char *flight_id, FL *flights);

#endif