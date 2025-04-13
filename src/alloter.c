#include "alloter.h"
#include "flights.h"
#include <sqlite3.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
Use
pragma foreign_keys = on
when inserting to enable foreign key constraint.
sqlite does not do it by default.
*/

void view_allots(sqlite3 *db, char *err_msg)
{
    // printf("Inside view allot fun\n");
    char *query = "SELECT * FROM alloted ORDER BY allotted_time;";

    printf("ID\tFLIGHT ID\tTIME\n");
    int rc = sqlite3_exec(db, query, view_allots_cb, 0, &err_msg);

    if (rc != SQLITE_OK) {

        fprintf(stderr, "Failed to select data\n");
        fprintf(stderr, "SQL error: %s\n", err_msg);

        sqlite3_free(err_msg);
        sqlite3_close(db);

        return;
    }
    return;
}

int view_allots_cb(void *NotUsed, int argc, char **argv, char **azColName)
{
    NotUsed = 0;

    printf("%s\t%s\t\t%s\n",
           argv[0] ? argv[0] : "NULL",
           argv[1] ? argv[1] : "NULL",
           argv[2] ? argv[2] : "NULL"
    );
    return 0;
}