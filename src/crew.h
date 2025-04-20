#ifndef CREW_H

#define CREW_H
#define INIT_CREW_SIZE 10

#include <stddef.h>

typedef struct crew_data{
    int crew_id;
    char name[100];
    char designation[50];
    char airline[50];
    int hours_worked; 
} CD;

typedef struct crew_list{
    CD *crew;
    size_t size;
    size_t allocated;
} CL;

void init_crew_list(CL *crew_list, size_t initial_size);
void addto_crew_list(CD *crew_data, CL *crew_list);
void free_crew_list(CL *crew_list);
int load_crew_data(CL *crew_list, sqlite3 *db, char *err_msg);
void load_crew_data_cb(void *cl, int argc, char **argv, char **azColName);

#endif