#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>

#define KCYN  "\x1B[36m"

int view_flight_schedules();
int view_flight_schedules_cb(void *, int, char **, char **);

void add_flight_schedules();
void update_flight_schedules();
void delete_flight_schedules();
void view_crew_info();

sqlite3 *flight_db;
char *err_msg = 0;

int main() {

    int rc = sqlite3_open("data/flights_small.db", &flight_db);

    if (rc != SQLITE_OK) {
        
        fprintf(stderr, "Cannot open database: %s\n", 
                sqlite3_errmsg(flight_db));
        sqlite3_close(flight_db);
        
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
                view_flight_schedules();
                break;
            case 2:
                add_flight_schedules();
                break;
            case 3:
                update_flight_schedules();
                break;
            case 4:
                delete_flight_schedules();
                break;
            case 5:
                view_crew_info();
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

int view_flight_schedules(){
    char *query = "SELECT * FROM flights";
    printf("-----------------------------------------------------------------------------\n");
    printf("%-12s %-12s %-8s %-12s %-15s %-15s\n", "Flight Id", "Airline", "Origin", "Destination", "Departure Time", "Arrival Time");
    printf("-----------------------------------------------------------------------------\n");
    int rc = sqlite3_exec(flight_db, query, view_flight_schedules_cb, 0, &err_msg);
    if (rc != SQLITE_OK ) {
        
        fprintf(stderr, "Failed to select data\n");
        fprintf(stderr, "SQL error: %s\n", err_msg);

        sqlite3_free(err_msg);
        sqlite3_close(flight_db);
        
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

void add_flight_schedules(){
    printf("Add Flight Schedule : Not yet implemented !!!\n");
}

void update_flight_schedules(){
    printf("Update Flight Schedule : Not yet implemented !!!\n");
}

void delete_flight_schedules(){
    printf("Delete Flight Schedule : Not yet implemented !!!\n");
} 

void view_crew_info(){
    printf("View Flight Crew Information : Not yet implemented !!!\n");
}
