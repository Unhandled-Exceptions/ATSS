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
        char *formatted_departure = g_strdup(format_time(flights.flight[i].departure_time));
        char *formatted_arrival = g_strdup(format_time(flights.flight[i].arrival_time));
        char *formatted_runway = g_strdup(format_time(flights.flight[i].runway_time));
        gtk_list_store_append(store, &iter);
        gtk_list_store_set(store, &iter, FI_FLIGHT_ID, flights.flight[i].flight_id, FI_AIRLINE, flights.flight[i].airline, FI_ORIGIN, flights.flight[i].origin, FI_DESTINATION, flights.flight[i].destination, FI_DEPARTURE, formatted_departure, FI_ARRIVAL, formatted_arrival, FI_AIRCRAFT_TYPE, flights.flight[i].aircraft_type, FI_PRIORITY_LEVEL, flights.flight[i].priority_level, FI_RUNWAY_TIME, formatted_runway, -1);
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

static void add_flight(GtkButton *button, gpointer user_data){
    int i=0;
    GtkWidget *parent_window = gtk_widget_get_toplevel(GTK_WIDGET(button));
    GtkWidget *dialog = gtk_dialog_new_with_buttons("Enter New Flight Details", GTK_WINDOW(parent_window), GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT, "_Cancel", GTK_RESPONSE_CANCEL, "_Create", GTK_RESPONSE_OK, NULL);

    gtk_widget_set_size_request(dialog, 500, 300);

    GtkWidget *content_area = gtk_dialog_get_content_area (GTK_DIALOG (dialog));
    GtkWidget *hbox = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 8);
    gtk_container_set_border_width (GTK_CONTAINER (hbox), 8);
    gtk_box_pack_start (GTK_BOX (content_area), hbox, FALSE, FALSE, 0);
  
    GtkWidget*image = gtk_image_new_from_icon_name ("emblem-new", GTK_ICON_SIZE_DIALOG);
    gtk_box_pack_start (GTK_BOX (hbox), image, FALSE, FALSE, 0);

    GtkWidget *grid = gtk_grid_new ();
    gtk_grid_set_row_spacing (GTK_GRID (grid), 4);
    gtk_grid_set_column_spacing (GTK_GRID (grid), 4);
    gtk_box_pack_start (GTK_BOX (hbox), grid, TRUE, TRUE, 0);

    GtkWidget *label_id = gtk_label_new("Flight ID:");
    GtkWidget *entry_id = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry_id), "e.g., FL075");  
    gtk_widget_set_hexpand(entry_id, TRUE);
    gtk_grid_attach(GTK_GRID(grid), label_id, 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), entry_id, 1, 0, 1, 1);

    GtkWidget *label_airline = gtk_label_new("Airline:");
    GtkWidget *combo_airline = gtk_combo_box_text_new();
    for (i=0; all_airlines[i] != NULL; i++){
        gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo_airline), all_airlines[i]);   
    }
    gtk_widget_set_hexpand(combo_airline, TRUE);
    gtk_grid_attach(GTK_GRID(grid), label_airline, 0, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), combo_airline, 1, 1, 1, 1);

    GtkWidget *label_origin = gtk_label_new("Origin:");
    GtkWidget *combo_origin = gtk_combo_box_text_new();
    for (i=0; all_airports[i] != NULL; i++){
        gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo_origin), all_airports[i]);   
    }
    gtk_widget_set_hexpand(combo_origin, TRUE);
    gtk_grid_attach(GTK_GRID(grid), label_origin, 0, 2, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), combo_origin, 1, 2, 1, 1);

    GtkWidget *label_destination = gtk_label_new("Destination:");
    GtkWidget *combo_destination = gtk_combo_box_text_new();
    for (i=0; all_airports[i] != NULL; i++){
        gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo_destination), all_airports[i]);   
    }
    gtk_widget_set_hexpand(combo_destination, TRUE);
    gtk_grid_attach(GTK_GRID(grid), label_destination, 0, 3, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), combo_destination, 1, 3, 1, 1);

    GtkWidget *label_departure = gtk_label_new("Departure:");
    GtkWidget *entry_departure = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry_departure), "e.g., 1430");
    gtk_entry_set_max_length(GTK_ENTRY(entry_departure), 4);
    gtk_widget_set_hexpand(entry_departure, TRUE);
    gtk_grid_attach(GTK_GRID(grid), label_departure, 0, 4, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), entry_departure, 1, 4, 1, 1);

    GtkWidget *label_arrival = gtk_label_new("Arrival:");
    GtkWidget *entry_arrival = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry_arrival), "e.g., 1830"); 
    gtk_entry_set_max_length(GTK_ENTRY(entry_arrival), 4);
    gtk_widget_set_hexpand(entry_arrival, TRUE);
    gtk_grid_attach(GTK_GRID(grid), label_arrival, 0, 5, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), entry_arrival, 1, 5, 1, 1);

    GtkWidget *label_aircraft_type = gtk_label_new("Aircraft Type:");
    GtkWidget *combo_aircraft_type = gtk_combo_box_text_new();
    for (i=0; all_aircraft_types[i] != NULL; i++){
        gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo_aircraft_type), all_aircraft_types[i]);   
    }
    gtk_widget_set_hexpand(combo_aircraft_type, TRUE);
    gtk_grid_attach(GTK_GRID(grid), label_aircraft_type, 0, 6, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), combo_aircraft_type, 1, 6, 1, 1);
    
    GtkWidget *label_priority_level = gtk_label_new("Priority Level:");
    GtkWidget *combo_priority_level = gtk_combo_box_text_new();
    gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(combo_priority_level), "1", "1 - Emergency");
    gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(combo_priority_level), "2", "2 - Domestic");
    gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(combo_priority_level), "3", "3 - International");
    gtk_grid_attach(GTK_GRID(grid), label_priority_level, 0, 7, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), combo_priority_level, 1, 7, 1, 1);

    gtk_widget_show_all(dialog);
    gint response = gtk_dialog_run(GTK_DIALOG(dialog));
    
    if (response == GTK_RESPONSE_OK) {
        char *id = g_strdup(gtk_entry_get_text(GTK_ENTRY(entry_id)));
        char *airline = g_strdup(gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(combo_airline)));
        char *origin = g_strdup(gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(combo_origin)));
        char *destination = g_strdup(gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(combo_destination)));
        char *departure = g_strdup(gtk_entry_get_text(GTK_ENTRY(entry_departure)));
        char *arrival = g_strdup(gtk_entry_get_text(GTK_ENTRY(entry_arrival)));
        char *aircraft_type = g_strdup(gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(combo_aircraft_type)));
        int priority_level = atoi(gtk_combo_box_get_active_id(GTK_COMBO_BOX(combo_priority_level)));

        if (strlen(id) == 0 || strlen(airline) == 0 || strlen(origin) == 0 || strlen(destination) == 0 || strlen(departure) == 0 || strlen(arrival) == 0 || strlen(aircraft_type) == 0){
            g_print("Please fill in all the fields\n");
            return;
        }

        FD new_flight;
        g_strlcpy(new_flight.flight_id, id, sizeof(new_flight.flight_id));
        g_strlcpy(new_flight.airline, airline, sizeof(new_flight.airline));
        g_strlcpy(new_flight.origin, origin, sizeof(new_flight.origin));
        g_strlcpy(new_flight.destination, destination, sizeof(new_flight.destination));
        g_strlcpy(new_flight.departure_time, departure, sizeof(new_flight.departure_time));
        g_strlcpy(new_flight.arrival_time, arrival, sizeof(new_flight.arrival_time));
        g_strlcpy(new_flight.aircraft_type, aircraft_type, sizeof(new_flight.aircraft_type));
        new_flight.priority_level = priority_level;

        TablewithDB *afd = (TablewithDB *)user_data;
        sqlite3 *db = afd->db;
        GtkWidget *table = afd->table;

        char *err_msg = 0;
        
        if (insert_flight_data(&new_flight, &flights, db, err_msg) != 0){
            g_print("Error adding flight\n");
            return;
        }
        GtkTreeModel *new_model = populate_flights_info_model();
        gtk_tree_view_set_model(GTK_TREE_VIEW(table), new_model);
        g_object_unref(new_model);
    }

    gtk_widget_destroy(dialog);
}

GtkWidget *create_flights_info_window(sqlite3 *db){

    char *err_msg = 0;
    init_flight_list(&flights, INIT_FLIGHTS_SIZE);

    if (load_flights_data(&flights, db, err_msg) != 0) {
        g_error("Failed to load flights data from database\n");
        sqlite3_close(db);
        exit(1);
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

    TablewithDB *afd = g_malloc(sizeof(TablewithDB));
    afd->db = db;
    afd->table = table;

    g_signal_connect (add_flight_btn, "clicked", G_CALLBACK (add_flight), afd);

    gtk_box_pack_start(GTK_BOX(win_box), scrolled_window, TRUE, TRUE, 0);


    return win_box;
}

// Flights Information Window - Ends

// Crew Information Window - Starts
static GtkTreeModel *populate_crew_info_model (){
    GtkListStore *store = gtk_list_store_new(CI_NUM_COLS, G_TYPE_UINT, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_UINT);

    GtkTreeIter iter;

    for (int i = 0; i < crew_list.size; i++){
        gtk_list_store_append(store, &iter);
        gtk_list_store_set(store, &iter, CI_CREW_ID, crew_list.crew[i].crew_id, CI_NAME, crew_list.crew[i].name, CI_DESIGNATION, crew_list.crew[i].designation, CI_AIRLINE, crew_list.crew[i].airline, CI_HOURS_WORKED, crew_list.crew[i].hours_worked, -1);
    }

    return GTK_TREE_MODEL (store);
}

static GtkWidget *create_crew_info_table (void){
    GtkWidget *table = gtk_tree_view_new();
    GtkCellRenderer *renderer;

    char *cols[] = {"Crew ID", "Name", "Designation","Airline", "Hours Worked"};

    for (int i = 0; i < CI_NUM_COLS; i++){
        renderer = gtk_cell_renderer_text_new();
        gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW (table), -1, cols[i], renderer,"text", i, NULL);
    }

    GtkTreeModel *model = populate_crew_info_model();
    gtk_tree_view_set_model (GTK_TREE_VIEW (table), model);

    g_object_unref (model);

    return table;
}

GtkWidget *create_crew_info_window(sqlite3 *db){

    char *err_msg = 0;
    init_crew_list(&crew_list, INIT_CREW_SIZE);

    if (load_crew_data(&crew_list, db, err_msg) != 0) {
        g_error("Failed to load crew data from database\n");
        sqlite3_close(db);
        exit(1);
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

    GtkWidget *add_crew_btn = gtk_button_new_with_label("Add Crew");
    GtkWidget *update_crew_btn = gtk_button_new_with_label("Update Crew");
    GtkWidget *delete_crew_btn = gtk_button_new_with_label("Delete Crew");
    
    gtk_container_add (GTK_CONTAINER (bbox), add_crew_btn);
    gtk_container_add (GTK_CONTAINER (bbox), update_crew_btn);
    gtk_container_add (GTK_CONTAINER (bbox), delete_crew_btn);

    // The Table !!

    GtkWidget *scrolled_window = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW(scrolled_window), GTK_SHADOW_IN); // Optional: adds a nice border

    GtkWidget *table = create_crew_info_table();
    gtk_container_add(GTK_CONTAINER(scrolled_window), table);

    gtk_box_pack_start(GTK_BOX(win_box), scrolled_window, TRUE, TRUE, 0);


    return win_box;
}
// Crew Information Window - Ends