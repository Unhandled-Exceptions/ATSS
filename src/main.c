#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>
#include <string.h>
#include "flights.h"
#include "crew.h"
#include "utils.h"

#define AIRPORT "MAA"

int view_flight_schedules(FL *flights);

void add_flight_schedules(FL *flights, sqlite3 *db, char *err_msg);
void update_flight_schedules(FL *flights, sqlite3 *db, char *err_msg);

void delete_flight_schedules(FL *flights, sqlite3 *db, char *err_msg);

int view_crew_info(CL *crew_list, char *err_msg);

int main(int argc, char const *argv[]) {

    sqlite3 *the_db;

    char *err_msg = 0;

    FL flights;
    init_flight_list(&flights, INIT_FLIGHTS_SIZE);

    CL crew_list;
    init_crew_list(&crew_list, INIT_CREW_SIZE);

    char db_name[100];
    char db_path[150];

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
        fprintf(stderr, "Failed to load flights data from database\n");
        sqlite3_close(the_db);
        return 1;
    }

    if (load_crew_data(&crew_list, the_db, err_msg) != 0) {
        fprintf(stderr, "Failed to load crew data from database\n");
        sqlite3_close(the_db);
        return 1;
    }
    
    int choice;
    while (1) {
        display_header();
        printf("\n\nMenu:\n1.View Flight Schedules\n2.Add Flight Schedules\n3.Update Flight Schedules\n4.Delete Flight Schedules\n5.View Flight Crew Information\n6.Exit\n");
        get_int_input("\n> ", &choice);
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
                view_crew_info(&crew_list, err_msg);
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
    clear_screen();
    printf("=========================================\n");
    printf("          Flight Schedule       \n");
    printf("=========================================\n\n");
    printf("------------------------------------------------------------------------------------------\n");
    printf("%-12s %-25s %-8s %-12s %-15s %-15s\n", 
           "Flight Id", "Airline", "Origin", "Destination", "Departure Time", "Arrival Time");
    printf("------------------------------------------------------------------------------------------\n");

    for (int i = 0; i < flights->size; i++) {
        char *dep = flights->flight[i].departure_time;
        char *arr = flights->flight[i].arrival_time;

        printf("%-12s %-25s %-8s %-12s %.2s:%.2s           %.2s:%.2s\n", 
            flights->flight[i].flight_id ? flights->flight[i].flight_id : "NULL",
            flights->flight[i].airline ? flights->flight[i].airline : "NULL",
            flights->flight[i].origin ? flights->flight[i].origin : "NULL",
            flights->flight[i].destination ? flights->flight[i].destination : "NULL",
            dep, dep + 2,
            arr, arr + 2);
    }

    printf("------------------------------------------------------------------------------------------\n\n");
    pauseScreen();
    return 0;
}

void add_flight_schedules(FL *flights, sqlite3 *db, char *err_msg){
        clear_screen();
        printf("=========================================\n");
        printf("          Add Flight Schedule       \n");
        printf("=========================================\n\n");
        FD flight;
        get_string_input("Enter Flight ID : ", flight.flight_id, 9);
        get_string_input("Enter Airline (max 24 chars) : ", flight.airline, 24);
        get_string_input("Enter Origin (IATA Code) : ", flight.origin,4);
        get_string_input("Enter Destination (IATA Code) : ", flight.destination,4);
        get_string_input("Enter Departure Time (HHMM) : ", flight.departure_time, 5);
        get_string_input("Enter Arrival Time (HHMM) : ", flight.arrival_time, 5);
        get_string_input("Enter Aircraft Type (max 30 chars) : ", flight.aircraft_type, 30);
        get_int_input("Enter new Priority Level (1 - Emergency, 2 - International, 3 - Domestic) : ", &flight.priority_level);

        if (insert_flight_data(&flight, flights, db, err_msg) != 0) {
            fprintf(stderr, "Failed to insert data into database\n");
        }

        pauseScreen();
}

void update_flight_schedules(FL *flights, sqlite3 *db, char *err_msg){
        clear_screen();
        printf("=========================================\n");
        printf("         Update Flight Schedule       \n");
        printf("=========================================\n\n");
        FD flight;
        get_string_input("Enter Flight ID : ", flight.flight_id, 10);
        get_string_input("Enter new Airline (max 24 chars) : ", flight.airline, 24);
        get_string_input("Enter new Origin (IATA Code) : ", flight.origin, 4);
        get_string_input("Enter new Destination (IATA Code) : ", flight.destination, 4);
        get_string_input("Enter new Departure Time (HHMM) : ", flight.departure_time, 5);
        get_string_input("Enter new Arrival Time (HHMM) : ", flight.arrival_time, 5);
        get_string_input("Enter new Aircraft Type (max 30 chars) : ", flight.aircraft_type, 30);
        get_int_input("Enter new Priority Level (1 - Emergency, 2 - International, 3 - Domestic) : ", &flight.priority_level);

        if (update_flight_data(&flight, flights, db, err_msg) != 0) {
            fprintf(stderr, "Failed to update data in database\n");
        }
        pauseScreen();
}

void delete_flight_schedules(FL *flights, sqlite3 *db, char *err_msg){
        clear_screen();
        printf("=========================================\n");
        printf("         Delete Flight Schedule       \n");
        printf("=========================================\n\n");
        char flight_id[10];
        get_string_input("Enter Flight ID : ", flight_id, 9);

        if (delete_flight_data(flight_id, flights, db, err_msg) != 0) {
            fprintf(stderr, "Failed to delete data from database\n");
        }
        pauseScreen();
} 

int view_crew_info(CL *crew_list, char *err_msg) {
    clear_screen();
    printf("==================================\n");
    printf("         Crew Information       \n");
    printf("==================================\n\n");

    printf("---------------------------------------------------------------------------------------------------------------\n");
    printf("%-8s %-40s %-15s %-20s %-15s\n", "Crew ID", "Name", "Designation", "Airline", "Hours Worked");
    printf("---------------------------------------------------------------------------------------------------------------\n");

    for (int i = 0; i < crew_list->size; i++) {
        printf("%-8d %-40s %-15s %-20s %-15d\n",
            crew_list->crew[i].crew_id,
            crew_list->crew[i].name ? crew_list->crew[i].name : "NULL",
            crew_list->crew[i].designation ? crew_list->crew[i].designation : "NULL",
            crew_list->crew[i].airline ? crew_list->crew[i].airline : "NULL",
            crew_list->crew[i].hours_worked);
    }

    printf("---------------------------------------------------------------------------------------------------------------\n");
    pauseScreen();
    return 0;
}