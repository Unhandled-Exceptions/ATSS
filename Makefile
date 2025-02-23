.PHONY: all
all: bin obj bin/atss

bin/atss: obj/main.o
	gcc obj/main.o -o bin/atss

bin:
	mkdir bin

obj/main.o: src/main.c
	gcc -c src/main.c -o obj/main.o

obj:
	mkdir obj

clean:
	rm -rf obj
	rm -rf bin
