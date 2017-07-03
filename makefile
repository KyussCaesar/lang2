debug: main.c
	gcc -ggdb main.c -o lang2

release: main.c
	gcc -O4 main.c -o lang2
