CC=gcc
CFLAGS = -I/usr/include/ncurses -lncurses 

sundial: main.o
	$(CC) -o sundial main.o $(CFLAGS)

main.o: main.c

clean: 
	rm -f sundial main.o