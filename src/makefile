CC=gcc
CFLAGS = -I/usr/include/ncurses -lncurses 

sundial: main.o
	$(CC) -o build/sundial main.o $(CFLAGS)

main.o: main.c

clean: 
	rm -f sundial main.o