CC = gcc -Wall
CFLAGS = -I lib/
LDFLAGS = -lpthread -ldl

GTK_CFLAGS = `pkg-config --cflags gtk+-3.0`
GTK_LDFLAGS = `pkg-config --libs gtk+-3.0`

SOURCES = src/main.c src/flights.c src/crew.c src/alloter.c src/crew_alloter.c src/utils.c lib/sqlite3.c
OBJECTS = $(SOURCES:.c=.o)

GUI_SOURCES = src/main_gui.c src/windows.c src/flights.c src/crew.c src/alloter.c src/crew_alloter.c src/utils.c lib/sqlite3.c
GUI_OBJECTS = $(GUI_SOURCES:.c=.o)

EXECUTABLE = bin/atss
GUI_EXECUTABLE = bin/atss-gui

all: $(EXECUTABLE) $(GUI_EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	mkdir -p bin
	$(CC) $(CFLAGS) $(OBJECTS) -o $@ $(LDFLAGS)

$(GUI_EXECUTABLE): $(GUI_OBJECTS)
	mkdir -p bin
	$(CC) $(CFLAGS) $(GTK_CFLAGS) $(GUI_OBJECTS) -o $@ $(LDFLAGS) $(GTK_LDFLAGS)

# Normal .o compilation (no GTK)
src/%.o lib/%.o: src/%.c lib/%.c
	mkdir -p bin
	$(CC) $(CFLAGS) -c $< -o $@

# GUI-specific .o compilation (with GTK CFLAGS)
src/main_gui.o: src/main_gui.c
	mkdir -p bin
	$(CC) $(CFLAGS) $(GTK_CFLAGS) -c $< -o $@

src/windows.o: src/windows.c
	mkdir -p bin
	$(CC) $(CFLAGS) $(GTK_CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(GUI_OBJECTS) $(EXECUTABLE) $(GUI_EXECUTABLE)
	rm -rf bin
