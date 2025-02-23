bin/atss: obj/main.o
	gcc obj/main.o -o bin/atss

obj/main.o: src/main.c
	gcc -c src/main.c -o obj/main.o

clean:
	rm -rf obj
	rm -rf bin
