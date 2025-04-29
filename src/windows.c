#include "windows.h"

struct delaydata delaydat;

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


// Flight Allotment Window - Starts

// Table related stuff
static int cb_populate_alloted_table_model(void *imusing, int argc, char **argv, char **azColName)
{
    struct allot_cb_pack *alcb = (struct allot_cb_pack *) imusing;

    // We'll put this in the callback
    char *formatted_runway = g_strdup(format_time(argv[2]));
    gtk_list_store_append(*alcb->store, alcb->iter);
    gtk_list_store_set(*alcb->store, alcb->iter, FA_ID, argv[0], FA_FLIGHT_ID, argv[1], FA_TIME, formatted_runway, FA_RUNWAY, argv[3], -1);
    // printf("%-6s %-12s %-8s %s\n",
    //        argv[0] ? argv[0] : "NULL",
    //        argv[1] ? argv[1] : "NULL",
    //        argv[2] ? argv[2] : "NULL",
    //        argv[3] ? argv[3] : "NULL");
    return 0;
}

static GtkTreeModel *populate_alloted_table_model (sqlite3 *db) {
    GtkListStore *store = gtk_list_store_new(FA_NUM_COLS, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING);
    GtkTreeIter iter;
    struct allot_cb_pack alcb = {&store, &iter};


    // SQL Section instead of a for loop we have cb function.
    char *err_msg = 0;
    char *query = "SELECT * FROM alloted ORDER BY allotted_time;";    
    int rc = sqlite3_exec(db, query, cb_populate_alloted_table_model, &alcb, &err_msg);

    if (rc != 0) {
        g_error("Failed to load alloted data from database\n");
        sqlite3_close(db);
        exit(1);
    }
    // SQL part over.
    return GTK_TREE_MODEL (store);
}

static void allot_flights(GtkButton *button, gpointer user_data) {
    // Unpacking the variabls
    TablewithDB *afd = (TablewithDB *)user_data;
    sqlite3 *the_db = afd->db;
    GtkWidget *table = afd->table;

    char err = 0;
    printf("allot_flights db pointer:  %p\n", (void*)the_db );
    fflush(stdout);
    delaydat = allotment(&flights, the_db, &err);

    // Updating the treeview.
    GtkTreeModel *new_model = populate_alloted_table_model(the_db);
    gtk_tree_view_set_model(GTK_TREE_VIEW(table), new_model);
    g_object_unref(new_model);
}

// Flight Delay
static void delay_flight(GtkButton *button, gpointer user_data) {

    TablewithDB *afd = (TablewithDB *)user_data;
    sqlite3 *the_db = afd->db;
    GtkWidget *table = afd->table;

    GtkWidget *parent_window = gtk_widget_get_toplevel(GTK_WIDGET(button));

    GtkWidget *dialog = gtk_dialog_new_with_buttons("Delay Flight",
                                                     GTK_WINDOW(parent_window),
                                                     GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
                                                     "_Cancel", GTK_RESPONSE_CANCEL,
                                                     "_Update", GTK_RESPONSE_OK,
                                                     NULL);

    GtkWidget *content_area = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
    gtk_container_set_border_width(GTK_CONTAINER(content_area), 10);

    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10); // 10 pixels spacing between rows
    gtk_box_pack_start(GTK_BOX(content_area), vbox, TRUE, TRUE, 0); // Pack vbox into content area

    // --- Flight ID Row ---
    GtkWidget *hbox_flight_id = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5); // 5 pixels spacing in this row
    gtk_box_pack_start(GTK_BOX(vbox), hbox_flight_id, FALSE, FALSE, 0); // Pack row into vbox

    GtkWidget *label_flight_id = gtk_label_new("Flight ID:");
    // Align label to the left
    gtk_widget_set_halign(label_flight_id, GTK_ALIGN_START);
    gtk_box_pack_start(GTK_BOX(hbox_flight_id), label_flight_id, FALSE, FALSE, 5); // Add padding after label

    GtkWidget *entry_flight_id = gtk_entry_new();
    gtk_box_pack_start(GTK_BOX(hbox_flight_id), entry_flight_id, TRUE, TRUE, 0); // Entry expands
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry_flight_id), "e.g., BA2490");


    // --- Delay Row ---
    GtkWidget *hbox_delay = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5); // 5 pixels spacing in this row
    gtk_box_pack_start(GTK_BOX(vbox), hbox_delay, FALSE, FALSE, 0); // Pack row into vbox

    GtkWidget *label_delay = gtk_label_new("Enter new time (HHMM):");
    gtk_widget_set_halign(label_delay, GTK_ALIGN_START);
    gtk_box_pack_start(GTK_BOX(hbox_delay), label_delay, FALSE, FALSE, 5); // Add padding after label

    GtkWidget *entry_delay = gtk_entry_new();
    gtk_box_pack_start(GTK_BOX(hbox_delay), entry_delay, TRUE, TRUE, 0); // Entry expands
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry_delay), "e.g., 1030");

    gtk_widget_set_size_request(dialog, 350, 200); // Increased size slightly

    gtk_widget_show_all(dialog);

    gint response = gtk_dialog_run(GTK_DIALOG(dialog));

    if (response == GTK_RESPONSE_OK) {
        const char *flight_id_text = gtk_entry_get_text(GTK_ENTRY(entry_flight_id));

        const char *delay_text = gtk_entry_get_text(GTK_ENTRY(entry_delay));
        int delay_value = atoi(delay_text); // Convert the text to an integer

        // char *flight_id;
        // strcpy(flight_id, flight_id_text);
        // char *delayed_time;
        // strcpy(delayed_time, delay_text);

        // TODO: Use flight_id_text (string) and delay_value (integer) here.
        // You would typically use these values to identify the specific flight
        // and update its delay in your data structure (afd) or database.
        // For example:
        // afd->current_flight_id = flight_id_text; // Assuming you have a way to store/find the flight
        // afd->delay = delay_value;
        // update_database(afd, flight_id_text, delay_value); // Call a function to update the DB

        g_print("Entered Flight ID: $%s$\n", flight_id_text);
        g_print("Entered delay: %d\n", delay_value); // Print for demonstration

        char err;
        remove_allot(flight_id_text, 1, the_db, &err);
        add_allot(flight_id_text, delay_text, 1, the_db, &err);
    }

    // Updating the treeview.
    GtkTreeModel *new_model = populate_alloted_table_model(the_db);
    gtk_tree_view_set_model(GTK_TREE_VIEW(table), new_model);
    g_object_unref(new_model);

    // Destroy the dialog
    gtk_widget_destroy(dialog);
}

// --- Callback Functions ---

// Callback for the "Delay" button
// Modified to use flight_identifier string
static void on_delay_flight_clicked(GtkButton *button, gpointer user_data) {
    FlightActionData *action_data = (FlightActionData *)user_data;
    // Ensure action_data and its members are valid before using them
    if (!action_data || !action_data->flight_identifier || !action_data->afd || !action_data->afd->db || !action_data->afd->table) {
         g_warning("Invalid user_data received in on_delay_flight_clicked");
         return;
    }

    const char *identifier = action_data->flight_identifier;
    TablewithDB *afd = action_data->afd;
    // GtkWidget *dialog_from_data = action_data->dialog; // Maybe needed for something else? Renamed to avoid conflict.
    sqlite3 *the_db = afd->db;
    GtkWidget *table = afd->table; // Assuming this is the GtkTreeView

    printf("Delay button clicked for flight identifier: %s\n", identifier);

    GtkWidget *parent_window = gtk_widget_get_toplevel(GTK_WIDGET(button));
    if (!parent_window) {
        g_warning("Could not get parent window for the dialog!");
        return; // Or handle error appropriately
    }
    // Check if the parent is actually a window
    if (!GTK_IS_WINDOW(parent_window)) {
         g_warning("Parent widget is not a GtkWindow!");
         // Find the actual parent window if necessary, or handle error
         parent_window = gtk_widget_get_ancestor(GTK_WIDGET(button), GTK_TYPE_WINDOW);
         if (!parent_window) {
             g_warning("Could not find ancestor GtkWindow!");
             return;
         }
    }


    // Create the NEW dialog locally
    GtkWidget *dialog = gtk_dialog_new_with_buttons("Delay Flight",
                                                     GTK_WINDOW(parent_window),
                                                     GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
                                                     "_Cancel", GTK_RESPONSE_CANCEL,
                                                     "_Update", GTK_RESPONSE_OK,
                                                     NULL);

    // Check if dialog creation succeeded
    if (!dialog) {
        g_critical("Failed to create the dialog window!");
        return;
    }


    GtkWidget *content_area = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
    gtk_container_set_border_width(GTK_CONTAINER(content_area), 10);

    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_box_pack_start(GTK_BOX(content_area), vbox, TRUE, TRUE, 0);

    // --- Delay Row ---
    GtkWidget *hbox_delay = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_box_pack_start(GTK_BOX(vbox), hbox_delay, FALSE, FALSE, 0);

    GtkWidget *label_delay = gtk_label_new("Enter new time (HHMM):");
    gtk_widget_set_halign(label_delay, GTK_ALIGN_START);
    gtk_box_pack_start(GTK_BOX(hbox_delay), label_delay, FALSE, FALSE, 5);

    GtkWidget *entry_delay = gtk_entry_new();
    gtk_box_pack_start(GTK_BOX(hbox_delay), entry_delay, TRUE, TRUE, 0);
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry_delay), "e.g., 1030");

    gtk_widget_set_size_request(dialog, 350, -1); // Often better to let height be automatic

    // Explicitly show the dialog and its contents BEFORE running it
    gtk_widget_show_all(dialog);

    g_print("Running the dialog...\n"); // Debug print
    gint response = gtk_dialog_run(GTK_DIALOG(dialog));
    g_print("Dialog response: %d\n", response); // Debug print

    if (response == GTK_RESPONSE_OK) {
        const char *delay_text = gtk_entry_get_text(GTK_ENTRY(entry_delay));
        // Add validation for delay_text (e.g., check format, length) here!
        int delay_value = atoi(delay_text); // Consider using strtol for better error checking

        g_print("Entered delay text: '%s', integer value: %d\n", delay_text ? delay_text : "(null)", delay_value);

        // Assuming remove_allot and add_allot handle potential NULL from delay_text if empty
        char err = 0; // Initialize err
        // IMPORTANT: Ensure remove_allot and add_allot are thread-safe if called from UI thread
        // and potentially interacting with background operations.
        remove_allot(identifier, 1, the_db, &err);
        // Handle potential error from remove_allot based on 'err'
        add_allot(identifier, delay_text, 1, the_db, &err);
         // Handle potential error from add_allot based on 'err'
    }

    // Updating the treeview.
    // Ensure populate_alloted_table_model is safe to call here (threading?)
    GtkTreeModel *new_model = populate_alloted_table_model(the_db);
    if (new_model) { // Check if model creation succeeded
        // Ensure 'table' is a valid GtkTreeView
        if (GTK_IS_TREE_VIEW(table)) {
             gtk_tree_view_set_model(GTK_TREE_VIEW(table), new_model);
             g_object_unref(new_model); // Unref only after setting it
        } else {
             g_warning("afd->table is not a GtkTreeView!");
             g_object_unref(new_model); // Unref if not used
        }
    } else {
        g_warning("Failed to populate new tree model.");
    }


    // Destroy the dialog explicitly AFTER gtk_dialog_run has returned
    // The GTK_DIALOG_DESTROY_WITH_PARENT flag handles destruction if the parent closes,
    // but destroying it here ensures it's cleaned up immediately after use.
    gtk_widget_destroy(dialog);
}

// Callback for the "Cancel" button
// Modified to use flight_identifier string
static void on_cancel_flight_clicked(GtkButton *button, gpointer user_data) {
    FlightActionData *action_data = (FlightActionData *)user_data;
    const char *identifier = action_data->flight_identifier;
    TablewithDB *afd = action_data->afd;
    GtkWidget *dialog = action_data->dialog; // Dialog is available if needed
    GtkWidget *row_widget = action_data->row_widget; // Get the row to remove

    printf("Cancel button clicked for flight identifier: %s\n", identifier);

    // --- Implement Cancel Logic (Database Update) ---
    // 1. Update the flight status to 'Cancelled' in the database (afd->db) for the flight matching 'identifier'.
    // 2. Handle potential database errors.
    // --- End of Cancel Logic ---

    // Remove the row widget from the dialog
    // This visually removes the entry from the list immediately.
    // Destroying the widget will also trigger the destruction of its children (label, buttons)
    // and associated signal data (like the FlightActionData via free_action_data).
    gtk_widget_destroy(row_widget);

    // No need to explicitly close the dialog here unless desired.
    // The user can still cancel/delay other flights or close the dialog manually.
}

// Function to free FlightActionData when the button is destroyed
// No change needed here as we only free the container struct, not the string it points to.
static void free_action_data(gpointer data, GClosure *closure) {
    free(data);
}

static void refresh_main_table_view(gpointer user_data) {
    TablewithDB *afd = (TablewithDB *)user_data;
    printf("Refreshing main table view...\n");
    GtkTreeModel *new_model = populate_alloted_table_model(afd->db);
    if (new_model) { // Check if model creation was successful
        gtk_tree_view_set_model(GTK_TREE_VIEW(afd->table), NULL); // Detach old model first
        gtk_tree_view_set_model(GTK_TREE_VIEW(afd->table), new_model);
        g_object_unref(new_model); // Dereference the model, treeview holds its own reference
    } else {
        fprintf(stderr, "Error: Failed to create new tree model for refresh.\n");
    }
}

// Conflict resolution
static void resolve_conflicts(GtkButton *button, gpointer user_data) {
    // Unpacking the variables
    TablewithDB *afd = (TablewithDB *)user_data;
    sqlite3 *the_db = afd->db; // Keep DB access if needed for callbacks
    GtkWidget *main_table = afd->table; // Main application table


    // Basic check
    if (delaydat.delaycount <= 0) {
         // Optionally show a message to the user
        GtkWidget *msg_dialog = gtk_message_dialog_new(NULL, // Parent window
                                         GTK_DIALOG_DESTROY_WITH_PARENT,
                                         GTK_MESSAGE_INFO,
                                         GTK_BUTTONS_OK,
                                         "No delayed flights to resolve.");
        gtk_dialog_run(GTK_DIALOG(msg_dialog));
        gtk_widget_destroy(msg_dialog);
        return; // Nothing to do
    }


    GtkWidget *parent_window = gtk_widget_get_toplevel(GTK_WIDGET(button));

    // Create the dialog - using "Close" instead of "Update"
    GtkWidget *dialog = gtk_dialog_new_with_buttons("Flight Resolution",
        GTK_WINDOW(parent_window),
        GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
        "_Close", GTK_RESPONSE_CLOSE, // Changed from OK/Cancel
        NULL);
    gtk_window_set_default_size(GTK_WINDOW(dialog), 600, 400); // Adjusted size

    // Get the content area of the dialog
    GtkWidget *content_area = gtk_dialog_get_content_area(GTK_DIALOG(dialog));

    // Create a scrolled window to hold the list
    GtkWidget *scrolled_window = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window),
                                   GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
    gtk_widget_set_vexpand(scrolled_window, TRUE); // Allow vertical expansion
    gtk_box_pack_start(GTK_BOX(content_area), scrolled_window, TRUE, TRUE, 0);

    // Create a vertical box to hold flight rows
    GtkWidget *list_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5); // Spacing between rows
    gtk_container_add(GTK_CONTAINER(scrolled_window), list_box);

    // --- Populate Flight List from delaypile ---
    // Iterate through the provided delaypile array
    for (int i = 0; i < delaydat.delaycount; i++) {
        // Create a horizontal box for each flight row
        GtkWidget *row_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10); // Spacing within row
        gtk_box_pack_start(GTK_BOX(list_box), row_box, FALSE, FALSE, 0);

        // Create label for flight identifier (using data from delaypile)
        char flight_details[100]; // Adjust size as needed
        snprintf(flight_details, sizeof(flight_details),
                 "Flight: %s",
                 delaydat.delaypile[i]); // Display the identifier from the array
        GtkWidget *label = gtk_label_new(flight_details);
        gtk_label_set_xalign(GTK_LABEL(label), 0.0); // Align text left
        gtk_box_pack_start(GTK_BOX(row_box), label, TRUE, TRUE, 0); // Label expands

        // --- Create Action Buttons ---
        GtkWidget *delay_button = gtk_button_new_with_label("Delay");
        GtkWidget *cancel_button = gtk_button_new_with_label("Cancel");

        // Allocate data for the callbacks
        // IMPORTANT: This memory needs to be freed. We use g_signal_connect_data
        // to free it when the button (the signal object) is destroyed.
        FlightActionData *delay_data = malloc(sizeof(FlightActionData));
        // Point directly to the string in the delaypile array.
        // Ensure delaypile's lifetime exceeds the dialog's lifetime.
        delay_data->flight_identifier = delaydat.delaypile[i];
        delay_data->afd = afd;
        delay_data->dialog = dialog;
        delay_data->row_widget = row_box; // Store the row widget


        FlightActionData *cancel_data = malloc(sizeof(FlightActionData));
        // Point directly to the string in the delaypile array.
        cancel_data->flight_identifier = delaydat.delaypile[i];
        cancel_data->afd = afd;
        cancel_data->dialog = dialog;
        cancel_data->row_widget = row_box; // Store the row widget

        // Connect signals, passing flight-specific data
        g_signal_connect_data(delay_button, "clicked",
                              G_CALLBACK(on_delay_flight_clicked),
                              delay_data, // Pass allocated data
                              free_action_data, // Function to free data
                              0);
        g_signal_connect_data(cancel_button, "clicked",
                              G_CALLBACK(on_cancel_flight_clicked),
                              cancel_data, // Pass allocated data
                              free_action_data, // Function to free data
                              0);


        // Pack buttons (pack end, non-expanding)
        gtk_box_pack_end(GTK_BOX(row_box), cancel_button, FALSE, FALSE, 0);
        gtk_box_pack_end(GTK_BOX(row_box), delay_button, FALSE, FALSE, 0);
    }
    // Removed freeing of 'flights' array as it's no longer used


    // --- Show Dialog and Handle Response ---
    gtk_widget_show_all(dialog);

    // Connect to the response signal to handle dialog closure
    g_signal_connect(dialog, "response", G_CALLBACK(gtk_widget_destroy), NULL);


    // --- Refresh Main Table (Moved) ---
    // Refreshing the main table should happen *after* an action is confirmed
    // or when the dialog is closed if changes were made.
    // It's better to trigger the refresh from the callbacks or upon dialog closure response
    // if changes occurred. For simplicity here, we refresh unconditionally after the
    // dialog is destroyed (which happens upon response).

    // We connect the refresh logic to the dialog's destroy signal
    g_signal_connect_swapped(dialog, "destroy",
                             G_CALLBACK(refresh_main_table_view),
                             afd); // Pass afd to the refresh function

    // gtk_widget_destroy(dialog); // Destroy is handled by the response signal now

}

// This calls populate_alloted_table_model()
static GtkWidget *create_alloted_table_widget (sqlite3 *db){
    GtkWidget *table = gtk_tree_view_new();
    GtkCellRenderer *renderer;

    char *cols[] = {"ID", "Flight ID", "Time", "Runway"};

    for (int i = 0; i < FA_NUM_COLS; i++){
        renderer = gtk_cell_renderer_text_new();
        gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW (table), -1, cols[i], renderer,"text", i, NULL);
    }

    GtkTreeModel *model = populate_alloted_table_model(db);
    gtk_tree_view_set_model (GTK_TREE_VIEW (table), model);

    g_object_unref (model);

    return table;
}

// Root view for the the alloter
GtkWidget *create_allot_window(sqlite3 *db) {
    
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

    GtkWidget *allot_flights_btn = gtk_button_new_with_label("Allot flights");
    GtkWidget *delay_flight_btn = gtk_button_new_with_label("Delay flight");
    GtkWidget *conflict_resolution_btn = gtk_button_new_with_label("Resolve Conflicts");

    gtk_container_add (GTK_CONTAINER (bbox), allot_flights_btn);
    gtk_container_add (GTK_CONTAINER (bbox), delay_flight_btn);
    gtk_container_add (GTK_CONTAINER (bbox), conflict_resolution_btn);
    
    // The Alloted table
    GtkWidget *scrolled_window = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW(scrolled_window), GTK_SHADOW_IN); // Optional: adds a nice border

    GtkWidget *table = create_alloted_table_widget(db);
    gtk_container_add(GTK_CONTAINER(scrolled_window), table);

    // Binding functions to buttons
    printf("create window db pointer:  %p\n", (void*)db );
    fflush(stdout);

    TablewithDB *afd = g_malloc(sizeof(TablewithDB));
    afd->db = db;
    afd->table = table;

    g_signal_connect (allot_flights_btn, "clicked", G_CALLBACK (allot_flights), afd);
    g_signal_connect (delay_flight_btn, "clicked", G_CALLBACK (delay_flight), afd);
    g_signal_connect (conflict_resolution_btn, "clicked", G_CALLBACK (resolve_conflicts), afd);


    gtk_box_pack_start(GTK_BOX(win_box), scrolled_window, TRUE, TRUE, 0);

    return win_box;
}

// Flight Allotment Window - Ends

// Runway Utilization Report - Starts
GtkWidget *create_report_window(sqlite3 *db) {
    GtkWidget *win_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 15);

    int margin = 15;
    gtk_widget_set_margin_start(win_box, margin);
    gtk_widget_set_margin_end(win_box, margin);
    gtk_widget_set_margin_top(win_box, margin);
    gtk_widget_set_margin_bottom(win_box, margin);

    GtkWidget *title_label = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(title_label), "<span size=\"large\" weight=\"bold\">Runway Utilization Report</span>");
    gtk_widget_set_halign(title_label, GTK_ALIGN_CENTER);

    GtkWidget *grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(grid), 10);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 10);
    gtk_widget_set_halign(grid, GTK_ALIGN_CENTER);

    char err_msg[256] = {0};
    struct report_data reportdat = utilization_report(&flights, db, err_msg);

    for (int i = 0; i < RUNWAYCOUNT; ++i) {

        char* usage_labeltext;
        asprintf(&usage_labeltext, "Runway No. %d\t Usage time", i + 1);
        GtkWidget *usage_label = gtk_label_new(usage_labeltext);
        GtkWidget *usage_entry = gtk_entry_new();

        char* usage_valuetext;
        asprintf(&usage_valuetext, "%d mins", reportdat.usage_time[i]);

        gtk_editable_set_editable(GTK_EDITABLE(usage_entry), FALSE);
        gtk_entry_set_text(GTK_ENTRY(usage_entry), usage_valuetext);
        gtk_widget_set_hexpand(usage_entry, TRUE);

        char* percentage_labeltext;
        asprintf(&percentage_labeltext, "Utilization", i + 1);
        GtkWidget *percentage_label = gtk_label_new(percentage_labeltext);
        GtkWidget *percentage_entry = gtk_entry_new();

        char* percentage_valuetext;
        int perc = reportdat.usage_time[i] * 100/ 1440;
        asprintf(&percentage_valuetext, "%d %%", perc);        
    
        gtk_editable_set_editable(GTK_EDITABLE(percentage_entry), FALSE);
        gtk_entry_set_text(GTK_ENTRY(percentage_entry), percentage_valuetext);
        gtk_widget_set_hexpand(percentage_entry, TRUE);

        gtk_grid_attach(GTK_GRID(grid), usage_label,       0, i, 1, 1);
        gtk_grid_attach(GTK_GRID(grid), usage_entry,       1, i, 1, 1);
        gtk_grid_attach(GTK_GRID(grid), percentage_label,  2, i, 1, 1);
        gtk_grid_attach(GTK_GRID(grid), percentage_entry,  3, i, 1, 1);

        free(usage_labeltext);
        free(usage_valuetext);
        free(percentage_labeltext);
        free(percentage_valuetext);
    }

    gtk_box_pack_start(GTK_BOX(win_box), title_label, FALSE, FALSE, 10);
    gtk_box_pack_start(GTK_BOX(win_box), grid, FALSE, FALSE, 0);

    return win_box;
}

// Runway Utilization Report - Ends



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

// Crew Alloter Window - Starts

static GtkTreeModel *populate_crew_allotments_model(sqlite3 *db) {
    GtkListStore *store = gtk_list_store_new(CA_NUM_COLS, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING);
    GtkTreeIter iter;

    const char *query =
        "SELECT ca.allotment_id, ca.flight_id, f.airline, c.crew_id, c.name, c.designation "
        "FROM crew_allotments ca "
        "JOIN flights f ON ca.flight_id = f.flight_id "
        "JOIN crew c ON ca.crew_id = c.crew_id "
        "ORDER BY ca.flight_id, c.designation, c.name;";

    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db, query, -1, &stmt, 0);

    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare crew allotment query: %s\n", sqlite3_errmsg(db));
        return GTK_TREE_MODEL(store);
    }

    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        const char *allot_id = (const char *)sqlite3_column_text(stmt, 0);
        const char *flight_id = (const char *)sqlite3_column_text(stmt, 1);
        const char *airline = (const char *)sqlite3_column_text(stmt, 2);
        const char *crew_id = (const char *)sqlite3_column_text(stmt, 3);
        const char *crew_name = (const char *)sqlite3_column_text(stmt, 4);
        const char *designation = (const char *)sqlite3_column_text(stmt, 5);

        gtk_list_store_append(store, &iter);
        gtk_list_store_set(store, &iter,
                           CA_ALLOTMENT_ID, allot_id ? allot_id : "",
                           CA_FLIGHT_ID, flight_id ? flight_id : "",
                           CA_AIRLINE, airline ? airline : "",
                           CA_CREW_ID, crew_id ? crew_id : "",
                           CA_CREW_NAME, crew_name ? crew_name : "",
                           CA_DESIGNATION, designation ? designation : "",
                           -1);
    }

    if (rc != SQLITE_DONE) {
         fprintf(stderr, "Error stepping through crew allotment results: %s\n", sqlite3_errmsg(db));
    }

    sqlite3_finalize(stmt);
    return GTK_TREE_MODEL(store);
}


static GtkWidget *create_crew_alloter_table(sqlite3 *db) {
    GtkWidget *table = gtk_tree_view_new();
    GtkCellRenderer *renderer;

    char *cols[] = {"AllocID", "Flight ID", "Airline", "Crew ID", "Crew Name", "Designation"};

    for (int i = 0; i < CA_NUM_COLS; i++) {
        renderer = gtk_cell_renderer_text_new();
        gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(table), -1, cols[i], renderer, "text", i, NULL);
    }

    GtkTreeModel *model = populate_crew_allotments_model(db);
    gtk_tree_view_set_model(GTK_TREE_VIEW(table), model);

    g_object_unref(model);

    return table;
}


static void allot_crew_button_cb(GtkButton *button, gpointer user_data) {
    TablewithDB *afd = (TablewithDB *)user_data;
    sqlite3 *db = afd->db;
    GtkWidget *table = afd->table;
    GtkWidget *parent_window = gtk_widget_get_toplevel(GTK_WIDGET(button));

    int success_count = 0;
    int fail_count = 0;
    char *err_msg = NULL;

    if (flights.size == 0 || crew_list.size == 0) {
         g_warning("Flight or Crew data not loaded. Cannot allot crew.");

         GtkWidget *error_dialog = gtk_message_dialog_new(GTK_WINDOW(parent_window), 
                                                      GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
                                                      GTK_MESSAGE_ERROR,
                                                      GTK_BUTTONS_CLOSE,
                                                      "Error: Flight or Crew data not loaded.\nPlease load data first (e.g., by visiting respective tabs).");
         gtk_dialog_run(GTK_DIALOG(error_dialog));
         gtk_widget_destroy(error_dialog);
         return;
    }

    int result = gui_allot_crew_for_flights(&flights, &crew_list, db, &success_count, &fail_count, &err_msg);

    GString *summary = g_string_new(NULL);
    if (result == 0) {
        g_string_append_printf(summary, "Crew Allotment Completed.\n\nFlights successfully assigned: %d\nFlights failed assignment: %d",
                               success_count, fail_count);
    } else {
        g_string_append_printf(summary, "Crew Allotment encountered errors.\n\nFlights successfully assigned: %d\nFlights failed assignment: %d\n\nError details: %s",
                               success_count, fail_count, (err_msg ? err_msg : "Unknown error"));
    }

    GtkMessageType message_type = (result == 0) ? GTK_MESSAGE_INFO : GTK_MESSAGE_WARNING;
    GtkWidget *summary_dialog = gtk_message_dialog_new(GTK_WINDOW(parent_window),
                                                       GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
                                                       message_type,
                                                       GTK_BUTTONS_OK,
                                                       "%s", summary->str);
    gtk_window_set_title(GTK_WINDOW(summary_dialog), "Allotment Summary");
    gtk_dialog_run(GTK_DIALOG(summary_dialog));
    gtk_widget_destroy(summary_dialog);

    g_string_free(summary, TRUE);
    if (err_msg) {
         g_free(err_msg);
    }

    GtkTreeModel *new_model = populate_crew_allotments_model(db);
    gtk_tree_view_set_model(GTK_TREE_VIEW(table), new_model);
    g_object_unref(new_model);

}

GtkWidget *create_crew_alloter_window(sqlite3 *db){

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

    GtkWidget *allot_crew_btn = gtk_button_new_with_label("Allot Crew");

    gtk_container_add (GTK_CONTAINER (bbox), allot_crew_btn);

    // The Table !!

    GtkWidget *scrolled_window = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW(scrolled_window), GTK_SHADOW_IN);

    GtkWidget *table = create_crew_alloter_table(db);
    gtk_container_add(GTK_CONTAINER(scrolled_window), table);

    TablewithDB *afd = g_malloc(sizeof(TablewithDB));
    afd->db = db;
    afd->table = table;

    g_signal_connect (allot_crew_btn, "clicked", G_CALLBACK (allot_crew_button_cb), afd);

    g_object_set_data_full(G_OBJECT(win_box), "afd_data_crew_alloter", afd, g_free);
    
    gtk_box_pack_start(GTK_BOX(win_box), scrolled_window, TRUE, TRUE, 0);


    return win_box;

}
// Crew Alloter Window - Ends