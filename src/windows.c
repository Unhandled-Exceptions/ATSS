#include "windows.h"
// Helper Functions - Start

// Convert time from HHMM to HH:MM
char* format_time(const char* time_hhmm) {
     static char formatted_time[6];

    formatted_time[0] = time_hhmm[0];
    formatted_time[1] = time_hhmm[1];
    formatted_time[2] = ':';
    formatted_time[3] = time_hhmm[2];
    formatted_time[4] = time_hhmm[3];
    formatted_time[5] = '\0';

    return formatted_time;
}
// Helper Functions - Ends

// Flights Information Window - Starts

static GtkTreeModel *populate_flights_info_model (){
    GtkListStore *store = gtk_list_store_new(FI_NUM_COLS, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_INT, G_TYPE_STRING);

    GtkTreeIter iter;

    for (int i = 0; i < flights.size; i++){
        gtk_list_store_append(store, &iter);
        gtk_list_store_set(store, &iter, FI_FLIGHT_ID, flights.flight[i].flight_id, FI_AIRLINE, flights.flight[i].airline, FI_ORIGIN, flights.flight[i].origin, FI_DESTINATION, flights.flight[i].destination, FI_DEPARTURE, format_time(flights.flight[i].departure_time), FI_ARRIVAL, format_time(flights.flight[i].arrival_time), FI_AIRCRAFT_TYPE, flights.flight[i].aircraft_type, FI_PRIORITY_LEVEL, flights.flight[i].priority_level, FI_RUNWAY_TIME, format_time(flights.flight[i].runway_time), -1);
    }
    // gtk_list_store_append(store, &iter);
    // gtk_list_store_set(store, &iter, FI_FLIGHT_ID, "FL002", FI_AIRLINE, "Air India", FI_ORIGIN, "BLR", FI_DESTINATION, "MAA", FI_DEPARTURE, format_time("0400"), FI_ARRIVAL, format_time("0700"), FI_AIRCRAFT_TYPE, "Boeing 747", FI_PRIORITY_LEVEL, 3, FI_RUNWAY_TIME, format_time("0700"), -1);

    return GTK_TREE_MODEL (store);
}

static GtkWidget *create_flights_info_table (void){
    GtkWidget *table = gtk_tree_view_new();
    GtkCellRenderer *renderer;

    char *cols[] = {"Flight ID", "Airline", "Origin", "Destination", "Departure", "Arrival", "Aircraft Type", "Priority Level", "Runway Time"};

    for (int i = 0; i < FI_NUM_COLS; i++){
        renderer = gtk_cell_renderer_text_new();
        gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW (table), -1, cols[i], renderer,"text", i, NULL);
    }

    GtkTreeModel *model = populate_flights_info_model();
    gtk_tree_view_set_model (GTK_TREE_VIEW (table), model);

    g_object_unref (model);

    return table;
}

GtkWidget *create_flights_info_window(sqlite3 *db){

    char *err_msg = 0;
    init_flight_list(&flights, INIT_FLIGHTS_SIZE);

    if (load_flights_data(&flights, db, err_msg) != 0) {
        g_error("Failed to load flights data from database\n");
        sqlite3_close(db);
        exit(0);
    }


    GtkWidget *win_box = gtk_box_new (GTK_ORIENTATION_VERTICAL, 10);;
    
    int margin = 15;
    gtk_widget_set_margin_start(win_box, margin);
    gtk_widget_set_margin_end(win_box, margin);
    gtk_widget_set_margin_top(win_box, margin);
    gtk_widget_set_margin_bottom(win_box, margin);

    GtkWidget *frame = gtk_frame_new ("Actions");

    GtkWidget *bbox = gtk_button_box_new (GTK_ORIENTATION_HORIZONTAL);
    gtk_container_set_border_width (GTK_CONTAINER (bbox), 10);

    gtk_container_add (GTK_CONTAINER (frame), bbox);
    gtk_box_pack_start(GTK_BOX(win_box), frame, FALSE, FALSE, 10); 
    
    gtk_button_box_set_layout (GTK_BUTTON_BOX (bbox), GTK_BUTTONBOX_END);
    gtk_box_set_spacing (GTK_BOX (bbox), 10);

    GtkWidget *add_flight_btn = gtk_button_new_with_label("Add Flight");
    GtkWidget *update_flight_btn = gtk_button_new_with_label("Update Flight");
    GtkWidget *delete_flight_btn = gtk_button_new_with_label("Delete Flight");
    
    gtk_container_add (GTK_CONTAINER (bbox), add_flight_btn);
    gtk_container_add (GTK_CONTAINER (bbox), update_flight_btn);
    gtk_container_add (GTK_CONTAINER (bbox), delete_flight_btn);

    // The Table !!

    GtkWidget *scrolled_window = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW(scrolled_window), GTK_SHADOW_IN); // Optional: adds a nice border

    GtkWidget *table = create_flights_info_table();
    gtk_container_add(GTK_CONTAINER(scrolled_window), table);

    gtk_box_pack_start(GTK_BOX(win_box), scrolled_window, TRUE, TRUE, 0);


    return win_box;
}

// Flights Information Window - Ends