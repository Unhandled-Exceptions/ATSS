// crew_alloter.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>
#include <stdbool.h>

#include "crew_alloter.h"
#include "flights.h" 
#include "crew.h"
#include "utils.h"

float calculate_flight_duration_hours(const char *departure_time, const char *arrival_time) {
    if (!departure_time || !arrival_time || strlen(departure_time) != 4 || strlen(arrival_time) != 4) {
        fprintf(stderr, "Error: Invalid time format for duration calculation.\n");
        return -1.0f;
    }

    int dep_h = (departure_time[0] - '0') * 10 + (departure_time[1] - '0');
    int dep_m = (departure_time[2] - '0') * 10 + (departure_time[3] - '0');
    int arr_h = (arrival_time[0] - '0') * 10 + (arrival_time[1] - '0');
    int arr_m = (arrival_time[2] - '0') * 10 + (arrival_time[3] - '0');

    if (dep_h < 0 || dep_h > 23 || dep_m < 0 || dep_m > 59 ||
        arr_h < 0 || arr_h > 23 || arr_m < 0 || arr_m > 59) {
        fprintf(stderr, "Error: Invalid time values for duration calculation.\n");
        return -1.0f;
    }

    int dep_total_mins = dep_h * 60 + dep_m;
    int arr_total_mins = arr_h * 60 + arr_m;

    int duration_mins;
    if (arr_total_mins >= dep_total_mins) {
        duration_mins = arr_total_mins - dep_total_mins;
    } else {
        duration_mins = (24 * 60 - dep_total_mins) + arr_total_mins;
    }

    return (float)duration_mins / 60.0f;
}

int create_crew_allotment_table(sqlite3 *db, char **err_msg) {
    const char *sql =
        "CREATE TABLE IF NOT EXISTS crew_allotments ("
        "  allotment_id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "  flight_id TEXT NOT NULL,"
        "  crew_id INTEGER NOT NULL,"
        "  assignment_time TIMESTAMP DEFAULT CURRENT_TIMESTAMP,"
        "  FOREIGN KEY (flight_id) REFERENCES flights(flight_id) ON DELETE CASCADE,"
        "  FOREIGN KEY (crew_id) REFERENCES crew(crew_id) ON DELETE CASCADE"
        ");";

    int rc = sqlite3_exec(db, sql, 0, 0, err_msg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error creating crew_allotments table: %s\n", *err_msg);
        return 1;
    }
    printf("Crew allotments table checked/created successfully.\n");
    return 0;
}


int allot_crew_for_flights(FL *flights, CL *crew_list, sqlite3 *db, char **err_msg) {
    clear_screen();
    printf("=========================================\n");
    printf("         Starting Crew Allotment       \n");
    printf("=========================================\n\n");

    if (create_crew_allotment_table(db, err_msg) != 0) {
        pauseScreen();
        return 1;
    }

    char *delete_sql = "DELETE FROM crew_allotments;";
    int rc = sqlite3_exec(db, delete_sql, 0, 0, err_msg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error clearing crew_allotments table: %s\n", *err_msg);
        sqlite3_free(*err_msg);
        *err_msg = NULL;
        pauseScreen();
        return 1;
    }
    printf("Previous crew allotments cleared.\n");

    AllottedFlightList allotted_flights;
    init_allotted_flight_list(&allotted_flights, 10); // Initial size
    char *select_sql = "SELECT DISTINCT flight_id FROM alloted ORDER BY allotted_time;";
    rc = sqlite3_exec(db, select_sql, get_allotted_flights_cb, &allotted_flights, err_msg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error fetching allotted flights: %s\n", *err_msg);
        sqlite3_free(*err_msg);
        *err_msg = NULL;
        free_allotted_flight_list(&allotted_flights);
        pauseScreen();
        return 1;
    }

    if (allotted_flights.size == 0) {
        printf("No flights found in the 'alloted' table to assign crew to.\n");
        free_allotted_flight_list(&allotted_flights);
        pauseScreen();
        return 0;
    }
    printf("Found %zu flights needing crew assignment.\n\n", allotted_flights.size);


    int successfully_assigned_count = 0;
    int failed_assignment_count = 0;
    bool *crew_assigned_in_this_run = calloc(crew_list->allocated, sizeof(bool));
    if (!crew_assigned_in_this_run) {
         fprintf(stderr, "Error: Memory allocation failed for crew tracking.\n");
         free_allotted_flight_list(&allotted_flights);
         pauseScreen();
         return 1;
    }

    int *original_hours = malloc(crew_list->size * sizeof(int));
     if (!original_hours) {
         fprintf(stderr, "Error: Memory allocation failed for original hours backup.\n");
         free(crew_assigned_in_this_run);
         free_allotted_flight_list(&allotted_flights);
         pauseScreen();
         return 1;
     }
     for(size_t i = 0; i < crew_list->size; ++i) {
         original_hours[i] = crew_list->crew[i].hours_worked;
     }


    for (size_t i = 0; i < allotted_flights.size; ++i) {
        const char *current_flight_id = allotted_flights.flights[i].flight_id;
        printf("-- Processing Flight: %s --\n", current_flight_id);

        FD *flight_details = find_flight_by_id(current_flight_id, flights);
        if (!flight_details) {
            fprintf(stderr, "  Warning: Details for flight %s not found in flight list. Skipping.\n", current_flight_id);
            failed_assignment_count++;
            continue;
        }

        float duration_h = calculate_flight_duration_hours(flight_details->departure_time, flight_details->arrival_time);
        if (duration_h < 0.0f) {
            fprintf(stderr, "  Warning: Could not calculate duration for flight %s. Skipping.\n", current_flight_id);
            failed_assignment_count++;
            continue;
        }
        printf("  Flight Duration: %.2f hours\n", duration_h);


        int pilots_found = 0;
        int attendants_found = 0;
        int assigned_crew_indices[REQUIRED_PILOTS + REQUIRED_ATTENDANTS]; // Store indices in crew_list
        int assigned_count = 0;

        bool *assigned_to_this_flight = calloc(crew_list->allocated, sizeof(bool));
         if (!assigned_to_this_flight) {
             fprintf(stderr, "  Error: Memory allocation failed for flight-specific crew tracking. Skipping %s.\n", current_flight_id);
             failed_assignment_count++;
             continue;
         }


         for (size_t j = 0; j < crew_list->size && pilots_found < REQUIRED_PILOTS; ++j) {
            CD *crew = &crew_list->crew[j];
            if (strcmp(crew->designation, "pilot") == 0 &&
                strcmp(crew->airline, flight_details->airline) == 0 &&
                (crew->hours_worked + (int)duration_h) <= MAX_HOURS_WORKED &&
                !crew_assigned_in_this_run[j] &&
                !assigned_to_this_flight[j] )
            {
                printf("    Found suitable Pilot: %s (ID: %d, Hrs: %d)\n", crew->name, crew->crew_id, crew->hours_worked);
                assigned_crew_indices[assigned_count++] = j;
                assigned_to_this_flight[j] = true;
                pilots_found++;
            }
        }

        for (size_t j = 0; j < crew_list->size && attendants_found < REQUIRED_ATTENDANTS; ++j) {
            CD *crew = &crew_list->crew[j];
            if (strcmp(crew->designation, "attendant") == 0 &&
                strcmp(crew->airline, flight_details->airline) == 0 &&
                (crew->hours_worked + (int)duration_h) <= MAX_HOURS_WORKED &&
                !crew_assigned_in_this_run[j] &&
                !assigned_to_this_flight[j] )
             {
                printf("    Found suitable Attendant: %s (ID: %d, Hrs: %d)\n", crew->name, crew->crew_id, crew->hours_worked);
                assigned_crew_indices[assigned_count++] = j;
                assigned_to_this_flight[j] = true;
                attendants_found++;
            }
        }

        free(assigned_to_this_flight);

        if (pilots_found == REQUIRED_PILOTS && attendants_found == REQUIRED_ATTENDANTS) {
            printf("  Successfully found required crew for flight %s.\n", current_flight_id);
            successfully_assigned_count++;

            for (int k = 0; k < assigned_count; ++k) {
                int crew_index = assigned_crew_indices[k];
                CD *assigned_crew = &crew_list->crew[crew_index];

                char *insert_sql_template = "INSERT INTO crew_allotments (flight_id, crew_id) VALUES ('%q', %d);";
                char *insert_sql = sqlite3_mprintf(insert_sql_template, current_flight_id, assigned_crew->crew_id);

                if (!insert_sql) {
                    fprintf(stderr, "   Error: Memory allocation failed for insert query. Assignment skipped for crew %d.\n", assigned_crew->crew_id);
                    continue;
                }

                rc = sqlite3_exec(db, insert_sql, 0, 0, err_msg);
                if (rc != SQLITE_OK) {
                    fprintf(stderr, "   SQL error inserting crew assignment (%s, %d): %s\n", current_flight_id, assigned_crew->crew_id, *err_msg);
                    sqlite3_free(*err_msg);
                    *err_msg = NULL;
                } else {
                    printf("    -> Assigned %s (ID: %d) to flight %s. Updating hours.\n", assigned_crew->name, assigned_crew->crew_id, current_flight_id);
                    assigned_crew->hours_worked += (int)(duration_h + 0.5f);
                    crew_assigned_in_this_run[crew_index] = true;
                }
                sqlite3_free(insert_sql);
            }
             printf("  ---------------------------------\n");

        } else {
            printf("  Failed to find sufficient crew for flight %s (Found %d/%d Pilots, %d/%d Attendants).\n",
                   current_flight_id, pilots_found, REQUIRED_PILOTS, attendants_found, REQUIRED_ATTENDANTS);
             printf("  ---------------------------------\n");
            failed_assignment_count++;
        }
    }


    printf("\nAttempting to save updated crew hours to database...\n");
    if (update_batch_crew_hours_in_db(crew_list, db, err_msg) != 0) {
         fprintf(stderr, "Error saving updated crew hours to the database. Check logs.\n");
          for(size_t k = 0; k < crew_list->size; ++k) {
              crew_list->crew[k].hours_worked = original_hours[k];
          }
          rc = 1;
    } else {
         printf("Updated crew hours saved successfully.\n");
         rc = 0;
    }


    printf("\n=========================================\n");
    printf("          Crew Allotment Summary       \n");
    printf("=========================================\n");
    printf("Flights requiring crew: %zu\n", allotted_flights.size);
    printf("Flights successfully assigned crew: %d\n", successfully_assigned_count);
    printf("Flights failed assignment: %d\n", failed_assignment_count);
    printf("=========================================\n\n");

    free(crew_assigned_in_this_run);
    free(original_hours);
    free_allotted_flight_list(&allotted_flights);
    pauseScreen();
    return rc;
}

int update_batch_crew_hours_in_db(CL *crew_list, sqlite3 *db, char **err_msg) {
    sqlite3_stmt *stmt = NULL;
    const char *sql = "UPDATE crew SET hours_worked = ? WHERE crew_id = ?;";
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

    if (rc != SQLITE_OK) {
        fprintf(stderr, "DB error preparing update statement: %s\n", sqlite3_errmsg(db));
        return 1;
    }

    sqlite3_exec(db, "BEGIN TRANSACTION;", 0, 0, err_msg);

    int overall_rc = 0;

    for (size_t i = 0; i < crew_list->size; ++i) {
        
        sqlite3_bind_int(stmt, 1, crew_list->crew[i].hours_worked);
        sqlite3_bind_int(stmt, 2, crew_list->crew[i].crew_id);

        rc = sqlite3_step(stmt);
        if (rc != SQLITE_DONE) {
            fprintf(stderr, "DB error updating hours for crew ID %d: %s\n", crew_list->crew[i].crew_id, sqlite3_errmsg(db));
            overall_rc = 1;
        }

        sqlite3_reset(stmt);
    }

    if (overall_rc == 0) {
        sqlite3_exec(db, "COMMIT;", 0, 0, err_msg);
    } else {
        sqlite3_exec(db, "ROLLBACK;", 0, 0, err_msg);
        fprintf(stderr, "Database update rolled back due to errors.\n");
    }

    sqlite3_finalize(stmt);
    return overall_rc;
}

void view_crew_allotments(sqlite3 *db, char **err_msg) {
    clear_screen();
    printf("===========================================================================\n");
    printf("                        Crew Allotment Schedule                            \n");
    printf("===========================================================================\n\n");

    const char *query =
        "SELECT ca.allotment_id, ca.flight_id, f.airline, c.crew_id, c.name, c.designation "
        "FROM crew_allotments ca "
        "JOIN flights f ON ca.flight_id = f.flight_id "
        "JOIN crew c ON ca.crew_id = c.crew_id "
        "ORDER BY ca.flight_id, c.designation, c.name;"; // Order for readability

    printf("---------------------------------------------------------------------------\n");
    printf("%-8s %-10s %-15s %-8s %-25s %-15s\n",
           "AllocID", "Flight ID", "Airline", "Crew ID", "Crew Name", "Designation");
    printf("---------------------------------------------------------------------------\n");

    int rc = sqlite3_exec(db, query, view_crew_allotments_cb, 0, err_msg);

    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to retrieve crew allotments\n");
        fprintf(stderr, "SQL error: %s\n", *err_msg);
        sqlite3_free(*err_msg);
        *err_msg = NULL;
    } else {
        printf("---------------------------------------------------------------------------\n");
    }

    pauseScreen();
}

int view_crew_allotments_cb(void *NotUsed, int argc, char **argv, char **azColName) {
    NotUsed = 0;
    printf("%-8s %-10s %-15s %-8s %-25s %-15s\n",
           argv[0] ? argv[0] : "NULL", // AllocID
           argv[1] ? argv[1] : "NULL", // Flight ID
           argv[2] ? argv[2] : "NULL", // Airline
           argv[3] ? argv[3] : "NULL", // Crew ID
           argv[4] ? argv[4] : "NULL", // Crew Name
           argv[5] ? argv[5] : "NULL"  // Designation
           );
    return 0;
}

static void init_allotted_flight_list(AllottedFlightList *list, size_t initial_size) {
    list->flights = malloc(initial_size * sizeof(AllottedFlightInfo));
    if (!list->flights) {
        perror("Failed to allocate memory for allotted flight list");
        exit(EXIT_FAILURE);
    }
    list->size = 0;
    list->allocated = initial_size;
}

static void add_to_allotted_flight_list(AllottedFlightList *list, const char *flight_id) {
    if (list->size == list->allocated) {
        list->allocated *= 2;
        AllottedFlightInfo *temp = realloc(list->flights, list->allocated * sizeof(AllottedFlightInfo));
        if (!temp) {
            perror("Failed to reallocate memory for allotted flight list");
            exit(EXIT_FAILURE);
        }
        list->flights = temp;
    }
    strncpy(list->flights[list->size].flight_id, flight_id, sizeof(list->flights[list->size].flight_id) - 1);
    list->flights[list->size].flight_id[sizeof(list->flights[list->size].flight_id) - 1] = '\0'; // Ensure null termination
    list->size++;
}

static void free_allotted_flight_list(AllottedFlightList *list) {
    if (list && list->flights) {
        free(list->flights);
        list->flights = NULL;
        list->size = 0;
        list->allocated = 0;
    }
}

static int get_allotted_flights_cb(void *data, int argc, char **argv, char **azColName) {
    AllottedFlightList *list = (AllottedFlightList *)data;
    if (argc > 0 && argv[0]) { // Expecting flight_id in the first column
        add_to_allotted_flight_list(list, argv[0]);
    }
    return 0;
}
