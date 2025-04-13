#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>
#include <string.h>
#include "flights.h"

#define LOGOCOLR "\x1B[36m"
#define AIRPORT "MAA"

int view_flight_schedules(FL *flights);

void add_flight_schedules(FL *flights, sqlite3 *db, char *err_msg);
void update_flight_schedules(FL *flights, sqlite3 *db, char *err_msg);

void delete_flight_schedules(FL *flights, sqlite3 *db, char *err_msg);

int view_crew_info(sqlite3 *db, char *err_msg);
int view_crew_info_cb(void *, int, char **, char **);

int allot(sqlite3 *db, char *err_msg);

int main(int argc, char const *argv[]) {

    sqlite3 *the_db;

    char *err_msg = 0;

    FL flights;
    init_flight_list(&flights, INIT_FLIGHTS_SIZE);

    char db_name[100];
    char db_path[150];

    // Argument handling for db
    if (argc != 2) {
        printf("Usage is\n`atss atss`\n");
        return 0;
    }
    strcpy(db_name, argv[1]);

    snprintf(db_path, sizeof(db_path), "data/%s.db", db_name);

    int rc = sqlite3_open(db_path, &the_db);

    if (rc != SQLITE_OK) {
        fprintf(stderr, "Cannot open database: %s\n", 
                sqlite3_errmsg(the_db));
        sqlite3_close(the_db);
        return 1;
    }

    if (load_flights_data(&flights, the_db, err_msg) != 0) {
        fprintf(stderr, "Failed to load data from database\n");
        sqlite3_close(the_db);
        return 1;
    }

    // Display the title and copyright
    printf("%s    ___  ________________\n   /   |/_  __/ ___/ ___/\n  / /| | / /  \\__ \\\\__ \\ \n / ___ |/ /  ___/ /__/ / \n/_/  |_/_/  /____/____/  \n\x1B[0m",LOGOCOLR);
    printf("\nCopyright (c) 2025  by R Uthaya Murthy, Varghese K James, Tarun S\n");
    
    printf("\n\nMenu:\n1.View Flight Schedules\n2.Add Flight Schedules\n3.Update Flight Schedules\n4.Delete Flight Schedules\n5.View Flight Crew Information\n6.Exit\n");
    
    int choice;
    while (1) {
        printf("\n> ");
        scanf("%d",&choice);
        
        switch(choice){
            case 1:
                view_flight_schedules(&flights);
                break;
            case 2:
                add_flight_schedules(&flights, the_db, err_msg);
                break;
            case 3:
                update_flight_schedules(&flights, the_db, err_msg);
                break;
            case 4:
                delete_flight_schedules(&flights, the_db, err_msg);
                break;
            case 5:
                view_crew_info(the_db, err_msg);
                break;
            case 6:
                free_flight_list(&flights);
                sqlite3_close(the_db);
                printf("Bye !\n");
                exit(0);
            default:
                printf("Invalid choice !\n");
        }
    }
    return 0;  
}


int view_flight_schedules(FL *flights){
    
    printf("------------------------------------------------------------------------------------------\n");
    printf("%-12s %-25s %-8s %-12s %-15s %-15s\n", "Flight Id", "Airline", "Origin", "Destination", "Departure Time", "Arrival Time");
    printf("------------------------------------------------------------------------------------------\n");
    for (int i = 0; i < flights->size; i++) {
        printf("%-12s %-25s %-8s %-12s %-15s %-15s\n", 
            flights->flight[0].flight_id ? flights->flight[i].flight_id : "NULL",
            flights->flight[0].airline ? flights->flight[i].airline : "NULL",
            flights->flight[0].origin ? flights->flight[i].origin : "NULL",
            flights->flight[0].destination ? flights->flight[i].destination : "NULL",
            flights->flight[0].departure_time ? flights->flight[i].departure_time : "NULL",
            flights->flight[0].arrival_time ? flights->flight[i].arrival_time : "NULL");   
    }
    printf("------------------------------------------------------------------------------------------\n");
    return 0;

}

void add_flight_schedules(FL *flights, sqlite3 *db, char *err_msg){
        FD flight;
        printf("Enter Flight ID : ");
        scanf("%9s", flight.flight_id);
        printf("Enter Airline (max 24 chars) : ");
        scanf("%24s", flight.airline);
        printf("Enter Origin (IATA Code) : ");
        scanf("%3s", flight.origin);
        printf("Enter Destination (IATA Code) : ");
        scanf("%3s", flight.destination);
        printf("Enter Departure Time (HHMM) : ");
        scanf("%5s", flight.departure_time);
        printf("Enter Arrival Time (HHMM) : ");
        scanf("%5s", flight.arrival_time);
        printf("Enter Aircraft Type (max 30 chars) : ");
        scanf("%30s", flight.aircraft_type);
        printf("Enter Priority Level : ");
        scanf("%d", &flight.priority_level);

        if (insert_flight_data(&flight, flights, db, err_msg) != 0) {
            fprintf(stderr, "Failed to insert data into database\n");
        }
}

void update_flight_schedules(FL *flights, sqlite3 *db, char *err_msg){
        FD flight;
        printf("Enter Flight ID : ");
        scanf("%9s", flight.flight_id);
        printf("Enter new Airline (max 24 chars) : ");
        scanf("%24s", flight.airline);
        printf("Enter new Origin (IATA Code) : ");
        scanf("%3s", flight.origin);
        printf("Enter new Destination (IATA Code) : ");
        scanf("%3s", flight.destination);
        printf("Enter new Departure Time (HHMM) : ");
        scanf("%5s", flight.departure_time);
        printf("Enter new Arrival Time (HHMM) : ");
        scanf("%5s", flight.arrival_time);
        printf("Enter new Aircraft Type (max 30 chars) : ");
        scanf("%30s", flight.aircraft_type);
        printf("Enter new Priority Level : ");
        scanf("%d", &flight.priority_level);

        if (update_flight_data(&flight, flights, db, err_msg) != 0) {
            fprintf(stderr, "Failed to update data in database\n");
        }
}

void delete_flight_schedules(FL *flights, sqlite3 *db, char *err_msg){
        char flight_id[10];
        printf("Enter Flight ID : ");
        scanf("%9s", flight_id);

        if (delete_flight_data(flight_id, flights, db, err_msg) != 0) {
            fprintf(stderr, "Failed to delete data from database\n");
        }
} 

int view_crew_info(sqlite3 *db, char *err_msg){
    char *query = "SELECT * FROM crew";
    
    printf("--------------------------------------------------------------------------------------------\n");
    printf("%-40s %-15s %-20s %-15s\n", "Name", "Designation", "Airline", "Hours Worked");
    printf("--------------------------------------------------------------------------------------------\n");    
    
    int rc = sqlite3_exec(db, query, view_crew_info_cb, 0, &err_msg);
    
    if (rc != SQLITE_OK ) {
        
        fprintf(stderr, "Failed to select data\n");
        fprintf(stderr, "SQL error: %s\n", err_msg);

        sqlite3_free(err_msg);
        sqlite3_close(db);
        
        return 1;
    }
    printf("--------------------------------------------------------------------------------------------\n");
    return 0;
}

int view_crew_info_cb(void *NotUsed, int argc, char **argv, 
    char **azColName) {

NotUsed = 0;

printf("%-40s %-15s %-20s %-15s\n", 
argv[0] ? argv[0] : "NULL", 
argv[1] ? argv[1] : "NULL", 
argv[2] ? argv[2] : "NULL", 
argv[3] ? argv[3] : "NULL");

return 0;
}