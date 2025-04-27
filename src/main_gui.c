/*
In VSCode use:
unset GTK_MODULES; 
To compile:
gcc `pkg-config --cflags gtk+-3.0` -o bin/atss-gui src/main_gui.c `pkg-config --libs gtk+-3.0`
To run:
./bin/atss-gui
*/

#include <gtk/gtk.h>

static GtkWidget* create_main_window(GtkApplication *app);
static void create_main_view(GtkWidget *window);
static void activate(GtkApplication *app, gpointer user_data);


int main(int argc, char *argv[]) {
    GtkApplication *app;
    int status;

    app = gtk_application_new("org.UnhandledExceptions.ATSS", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
    status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);

    return status;
}

static GtkWidget* create_main_window(GtkApplication *app) {
    GtkWidget *window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "ATSS by Unhandled Exceptions");
    gtk_widget_set_size_request(GTK_WIDGET(window), 1200, 750); // Minimum size
    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER); // Center the window
    return window;
}

static void create_main_view(GtkWidget *window){
    GtkWidget *main_box = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
    
    GtkWidget *sidebar = gtk_stack_sidebar_new ();
    gtk_box_pack_start (GTK_BOX (main_box), sidebar, FALSE, TRUE, 0);    

    GtkWidget *stack = gtk_stack_new ();
    gtk_stack_set_transition_type (GTK_STACK (stack), GTK_STACK_TRANSITION_TYPE_SLIDE_UP_DOWN);
    gtk_stack_sidebar_set_stack (GTK_STACK_SIDEBAR (sidebar), GTK_STACK (stack));
    
    GtkWidget *widget = gtk_separator_new (GTK_ORIENTATION_VERTICAL);
    gtk_box_pack_start (GTK_BOX(main_box), widget, FALSE, FALSE, 0);
    gtk_box_pack_start (GTK_BOX (main_box), stack, TRUE, TRUE, 0);

    GtkWidget *label1 = gtk_label_new("Flight Information Here !!");
    gtk_stack_add_named(GTK_STACK(stack), label1, "flight_info");
    gtk_container_child_set(GTK_CONTAINER(stack), label1, "title", "Flights Information", NULL);
    
    GtkWidget *label2 = gtk_label_new("Crew Information Here !!");
    gtk_stack_add_named(GTK_STACK(stack), label2, "crew_info");
    gtk_container_child_set(GTK_CONTAINER(stack), label2, "title", "Crew Information", NULL);

    GtkWidget *label3 = gtk_label_new("Flight Alloter Here !!");
    gtk_stack_add_named(GTK_STACK(stack), label3, "flight_alloter");
    gtk_container_child_set(GTK_CONTAINER(stack), label3, "title", "Flight Alloter", NULL);

    GtkWidget *label4 = gtk_label_new("Crew Alloter Here !!");
    gtk_stack_add_named(GTK_STACK(stack), label4, "crew_alloter");
    gtk_container_child_set(GTK_CONTAINER(stack), label4, "title", "Crew Alloter", NULL);

    GtkWidget *label5 = gtk_label_new("Flight Emergencies Here !!");
    gtk_stack_add_named(GTK_STACK(stack), label5, "flight_emergencies");
    gtk_container_child_set(GTK_CONTAINER(stack), label5, "title", "Flight Emergencies", NULL);

    GtkWidget *label7 = gtk_label_new(" Runway Utilisation and Delay Reports Here !!");
    gtk_stack_add_named(GTK_STACK(stack), label7, "reports");
    gtk_container_child_set(GTK_CONTAINER(stack), label7, "title", "Reports", NULL);

    gtk_container_add (GTK_CONTAINER (window), main_box);
}

static void activate(GtkApplication *app, gpointer user_data) {
    GtkWidget *window = create_main_window(app);
    create_main_view(window);
    gtk_widget_show_all(window);
}