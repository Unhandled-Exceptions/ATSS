#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>
#include <string.h>
#include "flights.h"

void init_flight_list(FL *flights, size_t initial_size){
    flights->flight = (FD *) malloc(initial_size * sizeof(FD));
    if (flights->flight == NULL) {
        printf("Memory allocation for the flight list failed :(\n");
        exit(1);
    }
    flights->size = 0;
    flights->allocated = initial_size;
}

void addto_flight_list(FD *flight_data, FL *flights){
    if (flights->size == flights->allocated) {
        flights->allocated *= 2;
        flights->flight = (FD *) realloc(flights->flight, flights->allocated * sizeof(FD));
        if (flights->flight == NULL) {
            printf("Memory allocation for the flight list failed :(\n");
            exit(1);
        }
    }
    flights->flight[flights->size] = *flight_data;
    flights->size++;
}

void free_flight_list(FL *flights){
    free(flights->flight);
    flights->size = 0;
    flights->allocated = 0;
}

int load_flights_data(FL *flights, sqlite3 *db, char *err_msg){

    char *query = "SELECT * FROM flights ORDER BY runway_time, priority_level;";
    
    int rc = sqlite3_exec(db, query, (void *) load_flights_data_cb, (void *) flights, &err_msg);
    
    if (rc != SQLITE_OK ) {
        
        fprintf(stderr, "Failed to select data\n");
        fprintf(stderr, "SQL error: %s\n", err_msg);

        sqlite3_free(err_msg);
        sqlite3_close(db);
        
        return 1;
    }
    return 0;
}

void load_flights_data_cb(void *fl, int argc, char **argv, char **azColName){
    FL *flights = (FL *) fl;

    FD flight_data;

    strcpy(flight_data.flight_id, argv[0]);
    strcpy(flight_data.airline, argv[1]);
    strcpy(flight_data.origin, argv[2]);
    strcpy(flight_data.destination, argv[3]);
    strcpy(flight_data.departure_time, argv[4]);
    strcpy(flight_data.arrival_time, argv[5]);
    strcpy(flight_data.aircraft_type, argv[6]);
    flight_data.priority_level = atoi(argv[7]);
    strcpy(flight_data.runway_time, argv[8]);

    addto_flight_list(&flight_data, flights);
}

int insert_flight_data(FD *flight, FL *flights, sqlite3 *db, char *err_msg){

    if (strcmp(flight->origin, AIRPORT) == 0) {
        strcpy(flight->runway_time, flight->departure_time);
    } 
    else if (strcmp(flight->destination, AIRPORT) == 0) {
        strcpy(flight->runway_time, flight->arrival_time);
    }

    char *query_template = "INSERT INTO flights VALUES ('%q', '%q', '%q', '%q', '%q', '%q', '%q', %d, '%q');";
    char *query = sqlite3_mprintf(query_template, flight->flight_id, flight->airline, flight->origin, flight->destination, flight->departure_time, flight->arrival_time, flight->aircraft_type, flight->priority_level, flight->runway_time);
    
    if (query == NULL) {
        printf("Memory allocation for the query failed :(\n");
    }

    int rc = sqlite3_exec(db, query, 0, 0, &err_msg);
    
    if (rc != SQLITE_OK) {
        printf("SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
        sqlite3_free(query);
        return 1;
    }
    else {
        free_flight_list(flights);
        init_flight_list(flights, INIT_FLIGHTS_SIZE);
        if (load_flights_data(flights, db, err_msg) != 0) {
            printf("Failed to reload data !!!\n");
            return 1;
        }
        printf("Data inserted successfully !!!\n");
    }

    sqlite3_free(query);
    return 0;
}

int update_flight_data(FD *flight, FL *flights, sqlite3 *db, char *err_msg){

    if (strcmp(flight->origin, AIRPORT) == 0) {
        strcpy(flight->runway_time, flight->departure_time);
    } 
    else if (strcmp(flight->destination, AIRPORT) == 0) {
        strcpy(flight->runway_time, flight->arrival_time);
    }

    char *query_template = "UPDATE flights SET airline = '%q', origin = '%q', destination = '%q', departure_time = '%q', arrival_time = '%q', aircraft_type = '%q', priority_level = %d, runway_time = '%q' WHERE flight_id = '%q'";
    char *query = sqlite3_mprintf(query_template, flight->airline, flight->origin, flight->destination, flight->departure_time, flight->arrival_time, flight->aircraft_type, flight->priority_level, flight->runway_time, flight->flight_id);

    if (query == NULL) {
        printf("Memory allocation for the query failed :(\n");
    }

    int rc = sqlite3_exec(db, query, 0, 0, &err_msg);
    
    if (rc != SQLITE_OK) {
        printf("SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
        sqlite3_free(query);
        return 1;
    }
    else {
        printf("Data updated successfully !!!\n");
        free_flight_list(flights);
        init_flight_list(flights, INIT_FLIGHTS_SIZE);
        if (load_flights_data(flights, db, err_msg) != 0) {
            printf("Failed to reload data !!!\n");
            return 1;
        }
    }

    sqlite3_free(query);
    return 0;
}

int delete_flight_data(char *flight_id, FL *flights, sqlite3 *db, char *err_msg) {
    char *query_template = "DELETE FROM flights WHERE flight_id = '%q'";
    char *query = sqlite3_mprintf(query_template, flight_id);

    if (query == NULL) {
        printf("Memory allocation for the query failed :(\n");
        return 1;
    }

    int rc = sqlite3_exec(db, query, 0, 0, &err_msg);
    
    if (rc != SQLITE_OK) {
        printf("SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
        sqlite3_free(query);
        return 1;
    }
    else{
        free_flight_list(flights);
        init_flight_list(flights, INIT_FLIGHTS_SIZE);
        if (load_flights_data(flights, db, err_msg) != 0) {
            printf("Failed to reload data !!!\n");
            sqlite3_free(query);
            return 1;
        }
    }

    printf("Flight entry removed successfully !\n");
    
    sqlite3_free(query);
    return 0;
}
