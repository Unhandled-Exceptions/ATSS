CC = gcc
CFLAGS = -I lib/
LDFLAGS = -lpthread -ldl
SOURCES = src/main.c src/flights.c src/alloter.c lib/sqlite3.c
OBJECTS = $(SOURCES:.c=.o)
EXECUTABLE = bin/atss

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	mkdir -p bin
	$(CC) $(CFLAGS) $(OBJECTS) -o $@ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(EXECUTABLE)
	rm -rf bin

# # gcc -I lib/ -o bin/main.out src/main.c lib/sqlite3.c -lpthread -ldl

# bin/atss: src/main.c | bin
# 	gcc -I lib/ -o bin/atss src/main.c lib/sqlite3.c -lpthread -ldl