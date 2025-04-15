#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>
#include <string.h>
#include "crew.h"

void init_crew_list(CL *crew_list, size_t initial_size){
    crew_list->crew = (CD *) malloc(initial_size * sizeof(CD));
    if (crew_list->crew == NULL) {
        printf("Memory allocation for the flight list failed :(\n");
        exit(1);
    }
    crew_list->size = 0;
    crew_list->allocated = initial_size;
}

void addto_crew_list(CD *crew_data, CL *crew_list){
    if (crew_list->size == crew_list->allocated) {
        crew_list->allocated *= 2;
        crew_list->crew = (CD *) realloc(crew_list->crew, crew_list->allocated * sizeof(CD));
        if (crew_list->crew == NULL) {
            printf("Memory allocation for the flight list failed :(\n");
            exit(1);
        }
    }
    crew_list->crew[crew_list->size] = *crew_data;
    crew_list->size++;
}

void free_crew_list(CL *crew_list){
    free(crew_list->crew);
    crew_list->size = 0;
    crew_list->allocated = 0;
}

int load_crew_data(CL *crew_list, sqlite3 *db, char *err_msg){
    char *query = "SELECT * FROM crew ORDER BY crew_id;";
    
    int rc = sqlite3_exec(db, query, (void *) load_crew_data_cb, (void *) crew_list, &err_msg);
    
    if (rc != SQLITE_OK ) {
        
        fprintf(stderr, "Failed to select data\n");
        fprintf(stderr, "SQL error: %s\n", err_msg);

        sqlite3_free(err_msg);
        sqlite3_close(db);
        
        return 1;
    }
    return 0;
}

void load_crew_data_cb(void *cl, int argc, char **argv, char **azColName){
    CL *crew_list = (CL *) cl;
    CD crew_data;
    crew_data.crew_id = atoi(argv[0]);
    strcpy(crew_data.name, argv[1]);
    strcpy(crew_data.designation, argv[2]);
    strcpy(crew_data.airline, argv[3]);
    crew_data.hours_worked = atoi(argv[4]);

    addto_crew_list(&crew_data, crew_list);
}

int update_crew_data(CD *crew, CL *crew_list, sqlite3 *db, char *err_msg){
    char *query_template = "UPDATE crew SET name = '%q', designation = '%q', airline = '%q', hours_worked = %d WHERE crew_id = %d;";
    char *query = sqlite3_mprintf(query_template, crew->name, crew->designation, crew->airline, crew->hours_worked, crew->crew_id);

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
        free_crew_list(crew_list);
        init_crew_list(crew_list, INIT_CREW_SIZE);
        if (load_crew_data(crew_list, db, err_msg) != 0) {
            printf("Failed to reload data !!!\n");
            return 1;
        }
    }
}

int insert_crew_data(CD *crew, CL *crew_list, sqlite3 *db, char *err_msg){
    char *query_template = "INSERT INTO crew (crew_id, name, designation, airline, hours_worked) VALUES (%d, '%q', '%q', '%q', %d);";
    char *query = sqlite3_mprintf(query_template, crew->crew_id, crew->name, crew->designation, crew->airline, crew->hours_worked); 

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
    else {
        free_crew_list(crew_list);
        init_crew_list(crew_list, INIT_CREW_SIZE);
        if (load_crew_data(crew_list, db, err_msg) != 0) {
            printf("Failed to reload data !!!\n");
            return 1;
        }
    }

}

int delete_crew_data(int crew_id, CL *crew_list, sqlite3 *db, char *err_msg) {
    char *query_template = "DELETE FROM crew WHERE crew_id = '%d'";
    char *query = sqlite3_mprintf(query_template, crew_id);

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
        free_crew_list(crew_list);
        init_crew_list(crew_list, INIT_CREW_SIZE);
        if (load_crew_data(crew_list, db, err_msg) != 0) {
            printf("Failed to reload data !!!\n");
            sqlite3_free(query);
            return 1;
        }
    }

    printf("Crew entry removed successfully !\n");
    
    sqlite3_free(query);
    return 0;
}