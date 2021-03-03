all:
	gcc -c fifo.c
	gcc main.c fifo.o -o tp2virtual -lm
	rm *.o