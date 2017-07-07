debug: main.c helpers.h token.h
	gcc -ggdb main.c -o lang2

release: main.c helpers.h token.h
	gcc -O4 main.c -o lang2

clean:
	rm lang2
