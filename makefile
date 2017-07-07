debug: main.c helpers.h token.h arrays.h ast.h ast.c
	gcc -ggdb main.c -o lang2

release: main.c helpers.h token.h arrays.h ast.h ast.c
	gcc -O4 main.c -o lang2

clean:
	rm lang2
