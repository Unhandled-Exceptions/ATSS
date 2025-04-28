#ifndef WINDOWS_H
#define WINDOWS_H

#include <gtk/gtk.h>
#include <stdlib.h> 
#include <sqlite3.h>
#include "flights.h"
#include "alloter.h"
#include "crew_alloter.h"
#include "crew.h"
#include "utils.h"

// Helpful Utils - Starts
typedef struct {
  sqlite3 *db;
  GtkWidget *table;
} TablewithDB;
// Helpful Utils End

// Flight Information Window - Starts

extern FL flights;
// Flight Info (FI) Table Columns
enum
{
  FI_FLIGHT_ID = 0,
  FI_AIRLINE,
  FI_ORIGIN,
  FI_DESTINATION,
  FI_DEPARTURE,
  FI_ARRIVAL,
  FI_AIRCRAFT_TYPE,
  FI_PRIORITY_LEVEL,
  FI_RUNWAY_TIME,
  FI_NUM_COLS
} ;

GtkWidget *create_flights_info_window(sqlite3 *db);

// Flights Information Window - Ends

// Crew Information Window - Starts

extern CL crew_list;

// Crew Info (CI) Table Columns
enum
{
  CI_CREW_ID = 0,
  CI_NAME,
  CI_DESIGNATION,
  CI_AIRLINE,
  CI_HOURS_WORKED,
  CI_NUM_COLS
} ;

GtkWidget *create_crew_info_window(sqlite3 *db);
// Crew Information Window - Ends
#endif
