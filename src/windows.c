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

static gboolean set_combo_box_text_active_by_text(GtkComboBoxText *combo, const char *text_to_find) {
    
    if (!text_to_find) return FALSE;

    GtkTreeModel *model = gtk_combo_box_get_model(GTK_COMBO_BOX(combo));
    GtkTreeIter iter;
    gboolean valid = gtk_tree_model_get_iter_first(model, &iter);
    int index = 0;

    while (valid) {
        char *current_text = NULL;
        gtk_tree_model_get(model, &iter, 0, &current_text, -1);

        if (current_text && strcmp(current_text, text_to_find) == 0) {
            gtk_combo_box_set_active(GTK_COMBO_BOX(combo), index);
            g_free(current_text);
            return TRUE;
        }

        g_free(current_text);
        valid = gtk_tree_model_iter_next(model, &iter);
        index++;
    }
    return FALSE;
}

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

static void fetch_flight_details_cb(GtkButton *button, gpointer user_data) {
    FetchCallbackData *widgets = (FetchCallbackData *)user_data;

    const char *flight_id = gtk_entry_get_text(GTK_ENTRY(widgets->entry_id));

    if (g_utf8_strlen(flight_id, -1) == 0) {

         g_warning("Please enter a Flight ID first to fetch details.");
        return;
    }

    FD* found_flight = find_flight_by_id(flight_id, &flights);

    if (found_flight) {
        set_combo_box_text_active_by_text(GTK_COMBO_BOX_TEXT(widgets->combo_airline), found_flight->airline);
        set_combo_box_text_active_by_text(GTK_COMBO_BOX_TEXT(widgets->combo_origin), found_flight->origin);
        set_combo_box_text_active_by_text(GTK_COMBO_BOX_TEXT(widgets->combo_destination), found_flight->destination);
        gtk_entry_set_text(GTK_ENTRY(widgets->entry_departure), found_flight->departure_time);
        gtk_entry_set_text(GTK_ENTRY(widgets->entry_arrival), found_flight->arrival_time);
        set_combo_box_text_active_by_text(GTK_COMBO_BOX_TEXT(widgets->combo_aircraft_type), found_flight->aircraft_type);

        char priority_id_str[3];
        g_snprintf(priority_id_str, sizeof(priority_id_str), "%d", found_flight->priority_level);
        gtk_combo_box_set_active_id(GTK_COMBO_BOX(widgets->combo_priority_level), priority_id_str);

    } else {
        g_warning("Flight ID '%s' not found.", flight_id);
    }
}

static void update_flight(GtkButton *button, gpointer user_data){
    int i=0;
    TablewithDB *afd = (TablewithDB *)user_data;
    GtkWidget *parent_window = gtk_widget_get_toplevel(GTK_WIDGET(button));

    GtkWidget *dialog = gtk_dialog_new_with_buttons("Update Flight Details",
                                                     GTK_WINDOW(parent_window),
                                                     GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
                                                     "_Cancel", GTK_RESPONSE_CANCEL,
                                                     "_Update", GTK_RESPONSE_OK,
                                                     NULL);

    gtk_widget_set_size_request(dialog, 550, 350);

    GtkWidget *content_area = gtk_dialog_get_content_area(GTK_DIALOG(dialog));

    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_set_border_width(GTK_CONTAINER(vbox), 10);
    gtk_box_pack_start(GTK_BOX(content_area), vbox, TRUE, TRUE, 0);

    GtkWidget *id_hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
    GtkWidget *label_id = gtk_label_new("Flight ID:");
    GtkWidget *entry_id = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry_id), "Enter ID and Fetch");
    GtkWidget *fetch_button = gtk_button_new_with_label("Fetch Details");
    gtk_widget_set_hexpand(entry_id, TRUE);
    gtk_box_pack_start(GTK_BOX(id_hbox), label_id, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(id_hbox), entry_id, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(id_hbox), fetch_button, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(vbox), id_hbox, FALSE, FALSE, 5);

    GtkWidget *grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(grid), 4);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 4);
    gtk_box_pack_start(GTK_BOX(vbox), grid, TRUE, TRUE, 0);

    GtkWidget *label_airline = gtk_label_new("Airline:");
    GtkWidget *combo_airline = gtk_combo_box_text_new();
    for (i=0; all_airlines[i] != NULL; i++) gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo_airline), all_airlines[i]);
    gtk_widget_set_hexpand(combo_airline, TRUE);
    gtk_grid_attach(GTK_GRID(grid), label_airline, 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), combo_airline, 1, 0, 1, 1);

    GtkWidget *label_origin = gtk_label_new("Origin:");
    GtkWidget *combo_origin = gtk_combo_box_text_new();
    for (i=0; all_airports[i] != NULL; i++) gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo_origin), all_airports[i]);
    gtk_widget_set_hexpand(combo_origin, TRUE);
    gtk_grid_attach(GTK_GRID(grid), label_origin, 0, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), combo_origin, 1, 1, 1, 1);

    GtkWidget *label_destination = gtk_label_new("Destination:");
    GtkWidget *combo_destination = gtk_combo_box_text_new(); 
    for (i=0; all_airports[i] != NULL; i++) gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo_destination), all_airports[i]);
    gtk_widget_set_hexpand(combo_destination, TRUE);
    gtk_grid_attach(GTK_GRID(grid), label_destination, 0, 2, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), combo_destination, 1, 2, 1, 1);

    GtkWidget *label_departure = gtk_label_new("Departure:");
    GtkWidget *entry_departure = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry_departure), "e.g., 1430");
    gtk_entry_set_max_length(GTK_ENTRY(entry_departure), 4);
    gtk_widget_set_hexpand(entry_departure, TRUE);
    gtk_grid_attach(GTK_GRID(grid), label_departure, 0, 3, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), entry_departure, 1, 3, 1, 1);

    GtkWidget *label_arrival = gtk_label_new("Arrival:");
    GtkWidget *entry_arrival = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry_arrival), "e.g., 1830");
    gtk_entry_set_max_length(GTK_ENTRY(entry_arrival), 4);
    gtk_widget_set_hexpand(entry_arrival, TRUE);
    gtk_grid_attach(GTK_GRID(grid), label_arrival, 0, 4, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), entry_arrival, 1, 4, 1, 1);

    GtkWidget *label_aircraft_type = gtk_label_new("Aircraft Type:");
    GtkWidget *combo_aircraft_type = gtk_combo_box_text_new(); 
    for (i=0; all_aircraft_types[i] != NULL; i++) gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo_aircraft_type), all_aircraft_types[i]);
    gtk_widget_set_hexpand(combo_aircraft_type, TRUE);
    gtk_grid_attach(GTK_GRID(grid), label_aircraft_type, 0, 5, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), combo_aircraft_type, 1, 5, 1, 1);

    GtkWidget *label_priority_level = gtk_label_new("Priority Level:");
    GtkWidget *combo_priority_level = gtk_combo_box_text_new();
    gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(combo_priority_level), "1", "1 - Emergency");
    gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(combo_priority_level), "2", "2 - International");
    gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(combo_priority_level), "3", "3 - Domestic");
    gtk_grid_attach(GTK_GRID(grid), label_priority_level, 0, 6, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), combo_priority_level, 1, 6, 1, 1);

    FetchCallbackData *fetch_data = g_malloc(sizeof(FetchCallbackData));
    fetch_data->entry_id = entry_id; 
    fetch_data->combo_airline = combo_airline;
    fetch_data->combo_origin = combo_origin;
    fetch_data->combo_destination = combo_destination;
    fetch_data->entry_departure = entry_departure;
    fetch_data->entry_arrival = entry_arrival;
    fetch_data->combo_aircraft_type = combo_aircraft_type;
    fetch_data->combo_priority_level = combo_priority_level;

    g_signal_connect(fetch_button, "clicked", G_CALLBACK(fetch_flight_details_cb), fetch_data);

    g_object_set_data_full(G_OBJECT(dialog), "fetch_data", fetch_data, g_free);

    gtk_widget_show_all(dialog);
    gint response = gtk_dialog_run(GTK_DIALOG(dialog));

    if (response == GTK_RESPONSE_OK) {
        const char *id_str = gtk_entry_get_text(GTK_ENTRY(entry_id));
        char *airline_str = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(combo_airline));
        char *origin_str = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(combo_origin));
        char *destination_str = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(combo_destination));
        const char *departure_str = gtk_entry_get_text(GTK_ENTRY(entry_departure));
        const char *arrival_str = gtk_entry_get_text(GTK_ENTRY(entry_arrival));
        char *aircraft_type_str = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(combo_aircraft_type));
        const char *priority_id = gtk_combo_box_get_active_id(GTK_COMBO_BOX(combo_priority_level));
        int priority_level_int = priority_id ? atoi(priority_id) : 0;

        if (strlen(id_str) == 0 || !airline_str || !origin_str || !destination_str || strlen(departure_str) != 4 || strlen(arrival_str) != 4 || !aircraft_type_str || priority_level_int == 0) {
            g_warning("Update failed: Please ensure all fields are filled correctly (Times: HHMM).");
            gtk_widget_destroy(dialog);
            return;
        }


        FD updated_flight;
        g_strlcpy(updated_flight.flight_id, id_str, sizeof(updated_flight.flight_id));
        g_strlcpy(updated_flight.airline, airline_str, sizeof(updated_flight.airline));
        g_strlcpy(updated_flight.origin, origin_str, sizeof(updated_flight.origin));
        g_strlcpy(updated_flight.destination, destination_str, sizeof(updated_flight.destination));
        g_strlcpy(updated_flight.departure_time, departure_str, sizeof(updated_flight.departure_time));
        g_strlcpy(updated_flight.arrival_time, arrival_str, sizeof(updated_flight.arrival_time));
        g_strlcpy(updated_flight.aircraft_type, aircraft_type_str, sizeof(updated_flight.aircraft_type));
        updated_flight.priority_level = priority_level_int;

        sqlite3 *db = afd->db;
        GtkWidget *table = afd->table;
        char *err_msg = 0;

        if (update_flight_data(&updated_flight, &flights, db, err_msg) != 0){
            g_warning("Error updating flight in database: %s", err_msg ? err_msg : "Unknown error");
            sqlite3_free(err_msg);
        } else {
            g_print("Flight updated successfully!\n");
            GtkTreeModel *new_model = populate_flights_info_model();
            gtk_tree_view_set_model(GTK_TREE_VIEW(table), new_model);
            g_object_unref(new_model);
        }

    }
    gtk_widget_destroy(dialog);
}

static void delete_flight_cb(GtkButton *button, gpointer user_data) {
    TablewithDB *afd = (TablewithDB *)user_data;
    GtkWidget *parent_window = gtk_widget_get_toplevel(GTK_WIDGET(button));

    GtkWidget *dialog = gtk_dialog_new_with_buttons("Delete Flight",
                                                     GTK_WINDOW(parent_window),
                                                     GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
                                                     "_Cancel", GTK_RESPONSE_CANCEL,
                                                     "_Delete", GTK_RESPONSE_OK, 
                                                     NULL);
    gtk_widget_set_size_request(dialog, 350, 150);
    GtkWidget *content_area = gtk_dialog_get_content_area(GTK_DIALOG(dialog));

    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_set_border_width(GTK_CONTAINER(vbox), 15);
    gtk_box_pack_start(GTK_BOX(content_area), vbox, TRUE, TRUE, 0);

    GtkWidget *hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
    GtkWidget *label_id = gtk_label_new("Flight ID to Delete:");
    GtkWidget *entry_id = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry_id), "e.g., FL075");
    gtk_widget_set_hexpand(entry_id, TRUE);

    gtk_box_pack_start(GTK_BOX(hbox), label_id, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(hbox), entry_id, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, FALSE, 0);

    gtk_widget_show_all(dialog);


    gint response = gtk_dialog_run(GTK_DIALOG(dialog));

    if (response == GTK_RESPONSE_OK) {
        const char *flight_id_to_delete = gtk_entry_get_text(GTK_ENTRY(entry_id));

        if (g_utf8_strlen(flight_id_to_delete, -1) == 0) {
            g_warning("Please enter a Flight ID to delete.");
            gtk_widget_destroy(dialog);
            return;
        }

        gchar *confirm_msg = g_strdup_printf("Are you sure you want to delete flight %s?", flight_id_to_delete);
        GtkWidget *confirm_dialog = gtk_message_dialog_new(GTK_WINDOW(dialog), // Parent to the delete dialog
                                                          GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
                                                          GTK_MESSAGE_QUESTION,
                                                          GTK_BUTTONS_YES_NO,
                                                          "%s", confirm_msg);
        gtk_window_set_title(GTK_WINDOW(confirm_dialog), "Confirm Deletion");
        g_free(confirm_msg);

        gint confirm_response = gtk_dialog_run(GTK_DIALOG(confirm_dialog));

        if (confirm_response == GTK_RESPONSE_YES) {
            char *err_msg = NULL; 
            if (delete_flight_data((char *)flight_id_to_delete, &flights, afd->db, err_msg) != 0) {
                g_warning("Error deleting flight %s: %s", flight_id_to_delete, err_msg ? err_msg : "Unknown error");
                sqlite3_free(err_msg); 
            } else {
                g_print("Flight %s deleted successfully.\n", flight_id_to_delete);
                GtkTreeModel *new_model = populate_flights_info_model();
                gtk_tree_view_set_model(GTK_TREE_VIEW(afd->table), new_model);
                g_object_unref(new_model);
            }
        }
        gtk_widget_destroy(confirm_dialog);
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
    g_signal_connect (update_flight_btn, "clicked", G_CALLBACK (update_flight), afd);
    g_signal_connect (delete_flight_btn, "clicked", G_CALLBACK (delete_flight_cb), afd);

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

static GtkWidget *create_crew_info_table (){
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

//  Flight Emergencies Window - Starts

static GtkTreeModel *populate_flight_emergencies_model (sqlite3 *db){
    GtkListStore *store = gtk_list_store_new(FE_NUM_COLS, G_TYPE_STRING, G_TYPE_STRING);

    GtkTreeIter iter;

    const char *sql = "SELECT flight_id, emergency_type FROM flight_emergencies ORDER BY flight_id;";
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, 0);

    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to fetch flight emergencies: %s\n", sqlite3_errmsg(db));
    }

    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        const char *flight_id = sqlite3_column_text(stmt, 0);
        const char *emergency_type = sqlite3_column_text(stmt, 1);
        gtk_list_store_append(store, &iter);
        gtk_list_store_set(store, &iter, FE_FLIGHT_ID, flight_id, FE_EMERGENCY_TYPE, emergency_type, -1);
    }

    sqlite3_finalize(stmt);

    return GTK_TREE_MODEL (store);
}

static GtkWidget *create_flight_emergencies_table (sqlite3 *db){
    GtkWidget *table = gtk_tree_view_new();
    GtkCellRenderer *renderer;

    char *cols[] = {"Flight ID                      ", "Emergency Type"};

    for (int i = 0; i < FE_NUM_COLS; i++){
        renderer = gtk_cell_renderer_text_new();
        gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW (table), -1, cols[i], renderer,"text", i, NULL);
    }

    GtkTreeModel *model = populate_flight_emergencies_model(db);
    gtk_tree_view_set_model (GTK_TREE_VIEW (table), model);

    g_object_unref (model);

    return table;
}

static void fetch_flight_for_emergency_cb(GtkButton *button, gpointer user_data) {
    DeclareEmergencyDialogData *dialog_data = (DeclareEmergencyDialogData *)user_data;

    const char *flight_id = gtk_entry_get_text(GTK_ENTRY(dialog_data->entry_id));

    if (g_utf8_strlen(flight_id, -1) == 0) {
        g_warning("Please enter a Flight ID to fetch details.");
        gtk_label_set_text(GTK_LABEL(dialog_data->label_fetched_airline), "-");
        gtk_label_set_text(GTK_LABEL(dialog_data->label_fetched_origin), "-");
        gtk_label_set_text(GTK_LABEL(dialog_data->label_fetched_dest), "-");
        dialog_data->fetched_flight = NULL;
        return;
    }

    // Use the global 'flights' list (ensure it's loaded)
    FD* found_flight = find_flight_by_id(flight_id, &flights);

    if (found_flight) {
        dialog_data->fetched_flight = found_flight; // Store pointer to the flight
        // Update labels in the dialog
        gtk_label_set_text(GTK_LABEL(dialog_data->label_fetched_airline), found_flight->airline);
        gtk_label_set_text(GTK_LABEL(dialog_data->label_fetched_origin), found_flight->origin);
        gtk_label_set_text(GTK_LABEL(dialog_data->label_fetched_dest), found_flight->destination);
        g_print("Fetched details for: %s\n", flight_id);

    } else {
        g_warning("Flight ID '%s' not found.", flight_id);
        gtk_label_set_text(GTK_LABEL(dialog_data->label_fetched_airline), "Not Found");
        gtk_label_set_text(GTK_LABEL(dialog_data->label_fetched_origin), "-");
        gtk_label_set_text(GTK_LABEL(dialog_data->label_fetched_dest), "-");
        dialog_data->fetched_flight = NULL;
    }
}

static void declare_emergency_cb(GtkButton *button, gpointer user_data) {
    TablewithDB *afd = (TablewithDB *)user_data;
    GtkWidget *parent_window = gtk_widget_get_toplevel(GTK_WIDGET(button));

    GtkWidget *dialog = gtk_dialog_new_with_buttons("Declare Flight Emergency",
                                                     GTK_WINDOW(parent_window),
                                                     GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
                                                     "_Cancel", GTK_RESPONSE_CANCEL,
                                                     "_Declare", GTK_RESPONSE_OK,
                                                     NULL);
    gtk_widget_set_size_request(dialog, 450, 400); // Adjusted size
    GtkWidget *content_area = gtk_dialog_get_content_area(GTK_DIALOG(dialog));

    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_set_border_width(GTK_CONTAINER(vbox), 15);
    gtk_box_pack_start(GTK_BOX(content_area), vbox, TRUE, TRUE, 0);

    // --- Flight ID Input and Fetch ---
    GtkWidget *id_hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
    GtkWidget *label_id = gtk_label_new("Flight ID:");
    GtkWidget *entry_id = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry_id), "Enter ID and Fetch");
    GtkWidget *fetch_button = gtk_button_new_with_label("Fetch Details");
    gtk_widget_set_hexpand(entry_id, TRUE);
    gtk_box_pack_start(GTK_BOX(id_hbox), label_id, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(id_hbox), entry_id, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(id_hbox), fetch_button, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(vbox), id_hbox, FALSE, FALSE, 5);

    GtkWidget *details_frame = gtk_frame_new("Flight Details");
    gtk_frame_set_label_align(GTK_FRAME(details_frame), 0.02, 0.5); 
    gtk_box_pack_start(GTK_BOX(vbox), details_frame, FALSE, FALSE, 5);

    GtkWidget *details_grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(details_grid), 5);
    gtk_grid_set_column_spacing(GTK_GRID(details_grid), 10);
    gtk_container_set_border_width(GTK_CONTAINER(details_grid), 10);
    gtk_container_add(GTK_CONTAINER(details_frame), details_grid);

    gtk_grid_attach(GTK_GRID(details_grid), gtk_label_new("Airline:"), 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(details_grid), gtk_label_new("Origin:"), 0, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(details_grid), gtk_label_new("Destination:"), 0, 2, 1, 1);

    GtkWidget *label_airline_val = gtk_label_new("-");
    GtkWidget *label_origin_val = gtk_label_new("-");
    GtkWidget *label_dest_val = gtk_label_new("-");
    gtk_widget_set_halign(label_airline_val, GTK_ALIGN_START);
    gtk_widget_set_halign(label_origin_val, GTK_ALIGN_START);
    gtk_widget_set_halign(label_dest_val, GTK_ALIGN_START);
    gtk_grid_attach(GTK_GRID(details_grid), label_airline_val, 1, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(details_grid), label_origin_val, 1, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(details_grid), label_dest_val, 1, 2, 1, 1);


    GtkWidget *emergency_frame = gtk_frame_new("Select Emergency Type");
     gtk_frame_set_label_align(GTK_FRAME(emergency_frame), 0.02, 0.5);
    gtk_box_pack_start(GTK_BOX(vbox), emergency_frame, FALSE, FALSE, 5);

    GtkWidget *radio_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_set_border_width(GTK_CONTAINER(radio_box), 10);
    gtk_container_add(GTK_CONTAINER(emergency_frame), radio_box);

    const char *emergency_types[] = {"Weather Issue", "Technical Issue", "Medical Emergency", "Security Concern", "Other Emergency"};
    int num_emergency_types = sizeof(emergency_types) / sizeof(emergency_types[0]);

    GtkWidget *radio_button = NULL;
    GSList *radio_group_list = NULL;
    for (int i = 0; i < num_emergency_types; ++i) {
        radio_button = gtk_radio_button_new_with_label(radio_group_list, emergency_types[i]);
        gtk_box_pack_start(GTK_BOX(radio_box), radio_button, FALSE, FALSE, 0);
        radio_group_list = gtk_radio_button_get_group(GTK_RADIO_BUTTON(radio_button));
    }

    DeclareEmergencyDialogData *dialog_data = g_malloc(sizeof(DeclareEmergencyDialogData));
    dialog_data->entry_id = entry_id;
    dialog_data->label_fetched_airline = label_airline_val;
    dialog_data->label_fetched_origin = label_origin_val;
    dialog_data->label_fetched_dest = label_dest_val;
    dialog_data->radio_group_box = radio_box;
    dialog_data->radio_group = radio_group_list;
    dialog_data->fetched_flight = NULL;
    dialog_data->db = afd->db;
    dialog_data->main_emergencies_table = afd->table;

    g_signal_connect(fetch_button, "clicked", G_CALLBACK(fetch_flight_for_emergency_cb), dialog_data);

    g_object_set_data_full(G_OBJECT(dialog), "dialog_data", dialog_data, g_free);

    gtk_widget_show_all(dialog);
    gint response = gtk_dialog_run(GTK_DIALOG(dialog));

    if (response == GTK_RESPONSE_OK) {
        DeclareEmergencyDialogData *d_data = g_object_get_data(G_OBJECT(dialog), "dialog_data");

        if (!d_data->fetched_flight) {
            g_warning("Cannot declare emergency: No valid flight details fetched.");
        } else {
            int selected_choice = -1;
            const char *selected_emergency_text = NULL;
            GSList *list_item = d_data->radio_group;
            int current_choice = 1;

            while (list_item) {
                GtkWidget *current_radio = GTK_WIDGET(list_item->data);
                if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(current_radio))) {
                    selected_choice = current_choice;
                    selected_emergency_text = gtk_button_get_label(GTK_BUTTON(current_radio));
                    break;
                }
                list_item = g_slist_next(list_item);
                current_choice++;
            }

            if (selected_choice == -1) {
                g_warning("Cannot declare emergency: Please select an emergency type.");
            } else {
                g_print("Declaring emergency for flight %s, type: %s (Choice %d)\n",
                        d_data->fetched_flight->flight_id,
                        selected_emergency_text,
                        selected_choice);

                int result = declare_flight_emergency(d_data->fetched_flight,
                                                     &flights, // Pass the global flights list
                                                     selected_choice,
                                                     d_data->db);

                if (result == 0) {
                    g_print("Emergency declared successfully for %s.\n", d_data->fetched_flight->flight_id);
                    GtkTreeModel *new_model = populate_flight_emergencies_model(d_data->db);
                    gtk_tree_view_set_model(GTK_TREE_VIEW(d_data->main_emergencies_table), new_model);
                    g_object_unref(new_model);

                    GtkTreeModel *flights_model = populate_flights_info_model();
                    
                    g_object_unref(flights_model);


                } else if (result == 1) {
                     g_message("Flight %s was already at emergency priority.", d_data->fetched_flight->flight_id);
                }
                 else {
                    g_warning("Failed to declare emergency for flight %s (Error code: %d). Check console/logs.", d_data->fetched_flight->flight_id, result);
                }
            }
        }
    }

    gtk_widget_destroy(dialog);
}

GtkWidget *create_flight_emergencies_window(sqlite3 *db){
    GtkWidget *win_box = gtk_box_new (GTK_ORIENTATION_VERTICAL, 10);;

    int margin = 40;
    gtk_widget_set_margin_start(win_box, margin+80);
    gtk_widget_set_margin_end(win_box, margin+80);
    gtk_widget_set_margin_top(win_box, margin);
    gtk_widget_set_margin_bottom(win_box, margin);

    GtkWidget *frame = gtk_frame_new ("Actions");

    GtkWidget *bbox = gtk_button_box_new (GTK_ORIENTATION_HORIZONTAL);
    gtk_container_set_border_width (GTK_CONTAINER (bbox), 10);

    gtk_container_add (GTK_CONTAINER (frame), bbox);
    gtk_box_pack_start(GTK_BOX(win_box), frame, FALSE, FALSE, 10);

    gtk_button_box_set_layout (GTK_BUTTON_BOX (bbox), GTK_BUTTONBOX_END);
    gtk_box_set_spacing (GTK_BOX (bbox), 10);

    GtkWidget *declare_emergency_btn = gtk_button_new_with_label("Declare Flight Emergency");

    gtk_container_add (GTK_CONTAINER (bbox), declare_emergency_btn);

    // The Table !!
    GtkWidget *scrolled_window = gtk_scrolled_window_new(NULL, NULL);

    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW(scrolled_window), GTK_SHADOW_IN);

    GtkWidget *table = create_flight_emergencies_table(db);
    gtk_container_add(GTK_CONTAINER(scrolled_window), table);

    TablewithDB *afd = g_malloc(sizeof(TablewithDB));
    afd->db = db;
    afd->table = table; 

    g_signal_connect (declare_emergency_btn, "clicked", G_CALLBACK (declare_emergency_cb), afd);

    g_object_set_data_full(G_OBJECT(win_box), "afd_data", afd, g_free);


    gtk_box_pack_start(GTK_BOX(win_box), scrolled_window, TRUE, TRUE, 0);

    return win_box;
}
// Flight Emergencies Window - Ends