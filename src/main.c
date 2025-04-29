#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>
#include <string.h>
#include <ctype.h>
#include "flights.h"
#include "alloter.h"
#include "crew_alloter.h"
#include "crew.h"
#include "utils.h"

#define AIRPORT "MAA"

int view_flight_schedules(FL *flights);

void add_flight_schedules(FL *flights, sqlite3 *db, char *err_msg);
void update_flight_schedules(FL *flights, sqlite3 *db, char *err_msg);

void delete_flight_schedules(FL *flights, sqlite3 *db, char *err_msg);

int view_crew_info(CL *crew_list, char *err_msg);
void handle_realtime_event(FL *flights, sqlite3 *db, char *err_msg);

void view_flight_emergencies(FL *flights, sqlite3 *db, char *err_msg);

int allot(sqlite3 *db, char *err_msg);

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
        printf("\n\nMenu:\n");
        printf("1. View Flight Schedules\n");
        printf("2. Add Flight Schedules\n");
        printf("3. Update Flight Schedules\n");
        printf("4. Delete Flight Schedules\n");
        printf("5. View Flight Crew Information\n");
        printf("6. Declare Flight Emergency\n");
        printf("7. View Declared Flight Emergnecies\n");
        printf("8. View Runway Allotments\n");
        printf("9. Allot Runways to Flights\n");
        printf("10. Allot Crew to Flights\n");
        printf("11. View Crew Allotments\n");
        printf("12. Runway Utilisation Report\n");
        printf("13. Exit\n"); 
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
                handle_realtime_event(&flights, the_db, err_msg);
                break;
            case 7:
                view_flight_emergencies(&flights, the_db, err_msg);
                break;
            case 8:
                view_allots(the_db, err_msg);
                break;
            case 9:
                allotment(&flights, the_db, err_msg);
                break;
            case 10: 
                printf("Starting crew allotment...\n");
                if (allot_crew_for_flights(&flights, &crew_list, the_db, &err_msg) == 0) {
                    printf("Reloading crew data after crew allotment...\n");
                    free_crew_list(&crew_list); // Free old list
                    init_crew_list(&crew_list, INIT_CREW_SIZE); // Reinitialize
                    if (load_crew_data(&crew_list, the_db, err_msg) != 0) {
                        fprintf(stderr, "WARNING: Failed to reload crew data after crew allotment!\n");
                    } else {
                        printf("Crew data reloaded.\n");
                    }
                } else {
                    fprintf(stderr, "Crew allotment process encountered errors.\n");
                    sqlite3_free(err_msg);
                    err_msg = NULL;
                }
                break;
           case 11:
               view_crew_allotments(the_db, &err_msg);
                if (err_msg) { 
                    sqlite3_free(err_msg);
                    err_msg = NULL;
                }
               break;
            case 12:
                utilization_report(&flights, the_db, err_msg);
                break;
           case 13:
               free_flight_list(&flights);
               free_crew_list(&crew_list);
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
    printf("-----------------------------------------------------------------------------------------------------\n");
    printf("%-12s %-25s %-10s %-8s %-12s %-15s %-15s\n","Flight Id", "Airline", "Priority", "Origin", "Destination", "Departure Time", "Arrival Time");
    printf("-----------------------------------------------------------------------------------------------------\n");

    for (int i = 0; i < flights->size; i++) {
        char *dep = flights->flight[i].departure_time;
        char *arr = flights->flight[i].arrival_time;

        char priority_str[11];

        switch (flights->flight[i].priority_level) {
            case 1:
                strcpy(priority_str, "Emergency");
                break;
            case 2:
                strcpy(priority_str, "Intl");
                break;
            case 3:
                strcpy(priority_str, "Domestic");
                break;
            default:
                strcpy(priority_str, "Unknown");
                break;
        }

        printf("%-12s %-25s %-10s %-8s %-12s %.2s:%.2s           %.2s:%.2s\n",
            flights->flight[i].flight_id ? flights->flight[i].flight_id : "NULL",
            flights->flight[i].airline ? flights->flight[i].airline : "NULL",
            priority_str,
            flights->flight[i].origin ? flights->flight[i].origin : "NULL",
            flights->flight[i].destination ? flights->flight[i].destination : "NULL",
            dep, dep + 2,
            arr, arr + 2);
    }

    printf("-----------------------------------------------------------------------------------------------------\n\n");
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

void delete_flight_schedules(FL *flights, sqlite3 *db, char *err_msg) {
    clear_screen();
    printf("=========================================\n");
    printf("         Delete Flight Schedule       \n");
    printf("=========================================\n\n");

    char flight_id_to_delete[10];
    char confirmation[10];

    get_string_input("Enter Flight ID to delete: ", flight_id_to_delete, sizeof(flight_id_to_delete));

    FD *flight_to_delete = find_flight_by_id(flight_id_to_delete, flights);

    if (flight_to_delete == NULL) {
        printf("\nError: Flight ID '%s' not found.\n", flight_id_to_delete);
        pauseScreen();
        return;
    }

    printf("\n--- Flight Details Found ---\n");
    printf("Flight ID:       %s\n", flight_to_delete->flight_id);
    printf("Airline:         %s\n", flight_to_delete->airline);
    printf("Origin:          %s\n", flight_to_delete->origin);
    printf("Destination:     %s\n", flight_to_delete->destination);
    printf("Departure Time:  %.2s:%.2s\n", flight_to_delete->departure_time, flight_to_delete->departure_time + 2);
    printf("Arrival Time:    %.2s:%.2s\n", flight_to_delete->arrival_time, flight_to_delete->arrival_time + 2);
    printf("Aircraft Type:   %s\n", flight_to_delete->aircraft_type);
    printf("Priority Level:  %d\n", flight_to_delete->priority_level);
    printf("-----------------------------\n\n");

    get_string_input("Are you sure you want to delete this flight? (y/n): ", confirmation, sizeof(confirmation));

    if (strlen(confirmation) > 0 && toupper(confirmation[0]) == 'Y') {
        printf("\nDeleting flight '%s'...\n", flight_id_to_delete);
        if (delete_flight_data(flight_id_to_delete, flights, db, err_msg) != 0) {
            fprintf(stderr, "Failed to delete data from database.\n");
        }
    } else {
        printf("\nDeletion cancelled.\n");
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

void handle_realtime_event(FL *flights, sqlite3 *db, char *err_msg) {
    clear_screen();
    printf("=========================================\n");
    printf("       Declare Flight Emergency     \n");
    printf("=========================================\n\n");

    char flight_id_to_update[10];
    int event_choice = -1;
    char confirmation[10];

    get_string_input("Enter Flight ID experiencing an event: ", flight_id_to_update, sizeof(flight_id_to_update));

    FD *flight_to_update = find_flight_by_id(flight_id_to_update, flights);

    if (flight_to_update == NULL) {
        printf("\nError: Flight ID '%s' not found.\n", flight_id_to_update);
        pauseScreen();
        return;
    }

    printf("\n--- Flight Details Found ---\n");
    printf("Flight ID:       %s\n", flight_to_update->flight_id);
    printf("Airline:         %s\n", flight_to_update->airline);
    printf("Origin:          %s\n", flight_to_update->origin);
    printf("Destination:     %s\n", flight_to_update->destination);
    printf("Current Priority:%d (1=Emerg, 2=Intl, 3=Dom)\n", flight_to_update->priority_level);
    printf("-----------------------------\n\n");

    printf("Select the type of event:\n");
    printf("1. Weather Issue\n");
    printf("2. Technical Issue\n");
    printf("3. Medical Emergency\n");
    printf("4. Security Concern\n");
    printf("5. Other Emergency\n");

    while (1) {
        get_int_input("\nEnter event type number: ", &event_choice);
        if (event_choice >= 1 && event_choice <= 5) {
            break;
        } 
        else {
            printf("Invalid choice. Please enter a number between 1 and 5.\n");
        }
    }
    printf("\n");
    get_string_input("Declare this flight as EMERGENCY (Priority 1)? (y/n): ", confirmation, sizeof(confirmation));

    if (strlen(confirmation) > 0 && toupper(confirmation[0]) == 'Y') {
        declare_flight_emergency(flight_to_update, flights, event_choice, db);   
    } 
    else {
        printf("\nPriority update cancelled. Emergency event not recorded.\n");
    }

    pauseScreen();
}

void view_flight_emergencies(FL *flights, sqlite3 *db, char *err_msg) {
    clear_screen();
    printf("=========================================\n");
    printf("      View Declared Flight Emergencies   \n");
    printf("=========================================\n\n");

    const char *sql = "SELECT flight_id, emergency_type FROM flight_emergencies ORDER BY flight_id;";
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);

    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to fetch flight emergencies: %s\n", sqlite3_errmsg(db));
        return;
    }

    printf("%-12s %-20s\n", "Flight ID", "Emergency Type");
    printf("--------------------------------\n");

    int found = 0;

    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        const unsigned char *flight_id = sqlite3_column_text(stmt, 0);
        const unsigned char *emergency_type = sqlite3_column_text(stmt, 1);

        printf("%-12s %-20s\n", flight_id, emergency_type);
        found = 1;
    }

    if (!found) {
        printf("\nNo emergencies declared yet.\n");
    }

    if (rc != SQLITE_DONE) {
        fprintf(stderr, "Error while reading flight emergencies: %s\n", sqlite3_errmsg(db));
    }

    sqlite3_finalize(stmt);
    pauseScreen();
}