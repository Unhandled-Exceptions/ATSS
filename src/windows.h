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

typedef struct {
  GtkWidget *entry_id;
  GtkWidget *combo_airline;
  GtkWidget *combo_origin;
  GtkWidget *combo_destination;
  GtkWidget *entry_departure;
  GtkWidget *entry_arrival;
  GtkWidget *combo_aircraft_type;
  GtkWidget *combo_priority_level;
} FetchCallbackData;
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

// Flight Emergencies Window - Starts
enum
{
  FE_FLIGHT_ID = 0,
  FE_EMERGENCY_TYPE,
  FE_NUM_COLS
};

typedef struct {
  GtkWidget *entry_id;
  GtkWidget *label_fetched_airline; // Label to display fetched airline
  GtkWidget *label_fetched_origin;  // Label to display fetched origin
  GtkWidget *label_fetched_dest;    // Label to display fetched destination
  GtkWidget *radio_group_box;      // Container for radio buttons
  GSList *radio_group;            // Head of the radio button group list
  FD *fetched_flight;             // Pointer to the fetched flight data (or NULL)
  sqlite3 *db;                    // Database connection passed from main window
  GtkWidget *main_emergencies_table; // The table in the main window to refresh
} DeclareEmergencyDialogData;

GtkWidget *create_flight_emergencies_window(sqlite3 *db);
// Flight Emergencies Window - Ends
#endif
