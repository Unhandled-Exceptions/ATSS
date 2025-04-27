#ifndef WINDOWS_H
#define WINDOWS_H

#include <gtk/gtk.h>
#include <sqlite3.h>


// Flight Information Window - Starts

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

GtkWidget *create_flights_info_window();
// Flights Information Window - Ends
#endif
