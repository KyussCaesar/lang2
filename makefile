debug: main.c helpers.h helpers.c token.h token.c arrays.h ast.h ast.c
	gcc -D DEBUG_BUILD=1 -ggdb main.c helpers.c token.c ast.c -o lang2_debug

release: main.c helpers.h helpers.c token.h token.c arrays.h ast.h ast.c
	gcc -D DEBUG_BUILD=0 -O4 main.c helpers.c token.c ast.c -o lang2_release

clean:
	rm lang2_debug lang2_release

linecount:
	cat *.c *.h | wc -l

todos:
	grep -nT TODO *.c *.h

