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

void update_flight_schedules(FL *flights, sqlite3 *db, char *err_msg) {
    clear_screen();
    printf("=========================================\n");
    printf("         Update Flight Schedule       \n");
    printf("=========================================\n\n");

    char flight_id_to_update[10];
    char input_buffer[100];
    int temp_int;

    get_string_input("Enter Flight ID to update: ", flight_id_to_update, sizeof(flight_id_to_update));

    FD *flight_to_update = find_flight_by_id(flight_id_to_update, flights);

    if (flight_to_update == NULL) {
        printf("\nError: Flight ID '%s' not found.\n", flight_id_to_update);
        pauseScreen();
        return;
    }

    FD updated_flight = *flight_to_update;

    printf("\n--- Flight Details ---\n");
    printf("Flight ID:       %s\n", updated_flight.flight_id); // ID cannot be changed
    printf("Airline:         %s\n", updated_flight.airline);
    printf("Origin:          %s\n", updated_flight.origin);
    printf("Destination:     %s\n", updated_flight.destination);
    printf("Departure Time:  %s\n", updated_flight.departure_time);
    printf("Arrival Time:    %s\n", updated_flight.arrival_time);
    printf("Aircraft Type:   %s\n", updated_flight.aircraft_type);
    printf("Priority Level:  %d\n", updated_flight.priority_level);
    printf("-----------------------------\n\n");
    printf("Enter new details or press Enter to keep current value.\n\n");

    printf("Current Airline: %s\n", flight_to_update->airline);
    get_string_input("Enter new Airline (max 24 chars): ", input_buffer, sizeof(input_buffer));
    if (strlen(input_buffer) > 0) {
        strncpy(updated_flight.airline, input_buffer, sizeof(updated_flight.airline) - 1);
        updated_flight.airline[sizeof(updated_flight.airline) - 1] = '\0'; // Ensure null termination
    }

    printf("\nCurrent Origin: %s\n", flight_to_update->origin);
    get_string_input("Enter new Origin (IATA Code): ", input_buffer, sizeof(input_buffer));
    if (strlen(input_buffer) > 0) {
        strncpy(updated_flight.origin, input_buffer, sizeof(updated_flight.origin) - 1);
        updated_flight.origin[sizeof(updated_flight.origin) - 1] = '\0';
    }

    printf("\nCurrent Destination: %s\n", flight_to_update->destination);
    get_string_input("Enter new Destination (IATA Code): ", input_buffer, sizeof(input_buffer));
    if (strlen(input_buffer) > 0) {
        strncpy(updated_flight.destination, input_buffer, sizeof(updated_flight.destination) - 1);
        updated_flight.destination[sizeof(updated_flight.destination) - 1] = '\0';
    }

    printf("\nCurrent Departure Time (HHMM): %s\n", flight_to_update->departure_time);
    get_string_input("Enter new Departure Time (HHMM): ", input_buffer, sizeof(input_buffer));
    if (strlen(input_buffer) > 0) {
        strncpy(updated_flight.departure_time, input_buffer, sizeof(updated_flight.departure_time) - 1);
        updated_flight.departure_time[sizeof(updated_flight.departure_time) - 1] = '\0';
    }

    printf("\nCurrent Arrival Time (HHMM): %s\n", flight_to_update->arrival_time);
    get_string_input("Enter new Arrival Time (HHMM): ", input_buffer, sizeof(input_buffer));
    if (strlen(input_buffer) > 0) {
        strncpy(updated_flight.arrival_time, input_buffer, sizeof(updated_flight.arrival_time) - 1);
        updated_flight.arrival_time[sizeof(updated_flight.arrival_time) - 1] = '\0';
    }

    printf("\nCurrent Aircraft Type: %s\n", flight_to_update->aircraft_type);
    get_string_input("Enter new Aircraft Type (max 30 chars): ", input_buffer, sizeof(input_buffer));
     if (strlen(input_buffer) > 0) {
        strncpy(updated_flight.aircraft_type, input_buffer, sizeof(updated_flight.aircraft_type) - 1);
         updated_flight.aircraft_type[sizeof(updated_flight.aircraft_type) - 1] = '\0';
     }


    printf("\nCurrent Priority Level: %d\n", flight_to_update->priority_level);
    while (1) {
        get_string_input("Enter new Priority Level (1-3) (or press Enter to keep): ", input_buffer, sizeof(input_buffer));
        if (strlen(input_buffer) == 0) {
            break;
        }

        char *endptr;
        long val = strtol(input_buffer, &endptr, 10);

        if (*input_buffer != '\0' && *endptr == '\0' && val >= 1 && val <= 3) {
             updated_flight.priority_level = (int)val;
             break;
        } else {
             printf("Invalid input. Please enter a number between 1 and 3, or press Enter.\n");
        }
    }


    if (update_flight_data(&updated_flight, flights, db, err_msg) != 0) {
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