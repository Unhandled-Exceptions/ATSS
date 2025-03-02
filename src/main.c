#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>
#include <string.h>

#define KCYN  "\x1B[36m"
#define AIRPORT "MAA"

int view_flight_schedules(sqlite3 *db, char *err_msg);
int view_flight_schedules_cb(void *, int, char **, char **);

void add_flight_schedules(sqlite3 *db, char *err_msg);
void update_flight_schedules();

void delete_flight_schedules(sqlite3 *db, char *err_msg);

int view_crew_info(sqlite3 *db, char *err_msg);
int view_crew_info_cb(void *, int, char **, char **);

int main(int argc, char const *argv[]) {

    sqlite3 *flight_db;
    sqlite3 *crew_db;

    char *err_msg = 0;

    char flights_db_name[100];
    char flights_db_path[150];

    char crew_db_name[100];
    char crew_db_path[150];

    // Argument handling for db
    if (argc != 3) {
        printf("Usage is\n`atss flights crew_small`\n");
        return 0;
    }
    strcpy(flights_db_name, argv[1]);
    strcpy(crew_db_name, argv[2]);

    snprintf(flights_db_path, sizeof(flights_db_path), "data/%s.db", flights_db_name);
    snprintf(crew_db_path, sizeof(crew_db_path), "data/%s.db", crew_db_name);

    int rc1 = sqlite3_open(flights_db_path, &flight_db);
    int rc2 = sqlite3_open(crew_db_path, &crew_db);

    if (rc1 != SQLITE_OK) {
        
        fprintf(stderr, "Cannot open database: %s\n", 
                sqlite3_errmsg(flight_db));
        sqlite3_close(flight_db);
        return 1;
    }
    if (rc2 != SQLITE_OK) {
        
        fprintf(stderr, "Cannot open database: %s\n", 
                sqlite3_errmsg(crew_db));
        sqlite3_close(crew_db);
        
        return 1;
    }

    // Display the title and copyright
    printf("%s    ___  ________________\n   /   |/_  __/ ___/ ___/\n  / /| | / /  \\__ \\\\__ \\ \n / ___ |/ /  ___/ /__/ / \n/_/  |_/_/  /____/____/  \n\x1B[0m",KCYN);
    printf("\nCopyright (c) 2025  by R Uthaya Murthy, Varghese K James, Tarun S\n");
    
    printf("\n\nMenu:\n1.View Flight Schedules\n2.Add Flight Schedules\n3.Update Flight Schedules\n4.Delete Flight Schedules\n5.View Flight Crew Information\n6.Exit\n");
    
    int choice;
    while (1) {
        printf("\n> ");
        scanf("%d",&choice);
        
        switch(choice){
            case 1:
                view_flight_schedules(flight_db, err_msg);
                break;
            case 2:
                add_flight_schedules(flight_db, err_msg);
                break;
            case 3:
                update_flight_schedules();
                break;
            case 4:
                delete_flight_schedules(flight_db, err_msg);
                break;
            case 5:
                view_crew_info(crew_db, err_msg);
                break;
            case 6:
                printf("Bye !\n");
                sqlite3_close(flight_db);
                exit(0);
            default:
                printf("Invalid choice !\n");
        }
    }
    return 0;  
}


// View flight schedule
int view_flight_schedules(sqlite3 *db, char *err_msg){
    
    char *query = "SELECT * FROM flights ORDER BY departure_time;";
    
    printf("-----------------------------------------------------------------------------\n");
    printf("%-12s %-12s %-8s %-12s %-15s %-15s\n", "Flight Id", "Airline", "Origin", "Destination", "Departure Time", "Arrival Time");
    printf("-----------------------------------------------------------------------------\n");
    
    int rc = sqlite3_exec(db, query, view_flight_schedules_cb, 0, &err_msg);
    
    if (rc != SQLITE_OK ) {
        
        fprintf(stderr, "Failed to select data\n");
        fprintf(stderr, "SQL error: %s\n", err_msg);

        sqlite3_free(err_msg);
        sqlite3_close(db);
        
        return 1;
    }
    printf("-----------------------------------------------------------------------------\n");
    return 0;

}

int view_flight_schedules_cb(void *NotUsed, int argc, char **argv, 
                    char **azColName) {
    
    NotUsed = 0;
    
    printf("%-12s %-12s %-8s %-12s %-15s %-15s\n", 
        argv[0] ? argv[0] : "NULL", 
        argv[1] ? argv[1] : "NULL", 
        argv[2] ? argv[2] : "NULL", 
        argv[3] ? argv[3] : "NULL", 
        argv[4] ? argv[4] : "NULL", 
        argv[5] ? argv[5] : "NULL");

    return 0;
}

void add_flight_schedules(sqlite3 *db, char *err_msg){
    char *query_template = "INSERT INTO flights VALUES (%Q, %Q, %Q, %Q, %Q, %Q, %Q, %d, %Q)";

    char flight_id[10], airline[25], origin[4], destination[4], departure_time[6], arrival_time[6], aircraft_type[31], runway_time[6];
    int priority_level;
    printf("Enter Flight ID : ");
    scanf("%9s",flight_id);
    printf("Enter Airline (max 24 chars) : ");
    scanf("%24s",airline);
    printf("Enter Origin (IATA Code) : ");
    scanf("%3s",origin);
    printf("Enter Destination (IATA Code) : ");
    scanf("%3s",destination);
    printf("Enter Departure Time (HH:MM) : ");
    scanf("%5s",departure_time);
    printf("Enter Arrival Time (HH:MM) : ");
    scanf("%5s",arrival_time);
    printf("Enter Aircraft Type (max 30 chars) : ");
    scanf("%30s",aircraft_type);
    printf("Enter Priority Level : ");
    scanf("%d",&priority_level);
    
    if (strcmp(origin, AIRPORT)) {
        strcpy(runway_time, departure_time);
    } else if (strcmp(destination, AIRPORT)) {
        strcpy(runway_time, arrival_time);
    }

    char *query = sqlite3_mprintf(query_template, flight_id, airline, origin, destination, departure_time, arrival_time, aircraft_type, priority_level, runway_time);
    if (query == NULL) {
        printf("Memory allocation for the query failed :(\n");
    }

    int rc = sqlite3_exec(db, query, 0, 0, &err_msg);

    if (rc != SQLITE_OK) {
        printf("SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
    } else {
        printf("Data inserted successfully !!!\n");
    }

    sqlite3_free(query);

}

void update_flight_schedules(){
    printf("Update Flight Schedule : Not yet implemented !!!\n");
}

void delete_flight_schedules(sqlite3 *db, char *err_msg){
    char *query_template = "DELETE FROM flights WHERE flight_id = %Q";

    char flight_id[10];
    printf("Enter Flight ID : ");
    scanf("%9s",flight_id);
    
    char *query = sqlite3_mprintf(query_template, flight_id);
    if (query == NULL) {
        printf("Memory allocation for the query failed :(\n");
    }

    int rc = sqlite3_exec(db, query, 0, 0, &err_msg);
    
    if (rc != SQLITE_OK) {
        printf("SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
    } else {
        printf("Data entry removed successfully !\n");
    }

    sqlite3_free(query);
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