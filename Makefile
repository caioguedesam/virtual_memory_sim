all:
	gcc -c paging.c
	gcc main.c paging.o -o tp2virtual -lm
	rm *.o