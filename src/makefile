CC=gcc
# CFLAGS = -I/usr/include/ncurses -lncurses
CFLAGS = -I/usr/local/include/ncurses -lncurses

sundial: main.o scaling.o sun.o types.o
	$(CC) -o build/sundial main.o scaling.o sun.o types.o $(CFLAGS)

main.o: main.c

scaling.o: scaling.c

sun.o: sun.c

types.o: types.c

clean: 
	rm -f sundial main.o scaling.o sun.o types.o