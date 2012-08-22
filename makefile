CC=gcc
CFLAGS = -I/usr/include/ncurses

sundial: main.o
	$(CC) -o sundial main.o $(CFLAGS)

main.o: main.c

clean: 
	rm -f sundial main.o