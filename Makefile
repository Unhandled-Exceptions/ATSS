# gcc -I lib/ -o bin/main.out src/main.c lib/sqlite3.c -lpthread -ldl

bin/atss: src/main.c | bin
	gcc -I lib/ -o bin/atss src/main.c lib/sqlite3.c -lpthread -ldl

bin:
	mkdir bin

clean:
	rm -rf bin
