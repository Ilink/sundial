CC=gcc
BUILD_DIR = ../build
CFLAGS = -I/usr/include/ncurses -lncurses -g -lm
# CFLAGS = -I/usr/local/include/ncurses -lncurses

sundial: main.o drawing.o sun.o types.o util.o
	$(CC) -o $(BUILD_DIR)/sundial main.o drawing.o sun.o types.o util.o $(CFLAGS)

main.o: main.c

util.o: util.c

drawing.o: drawing.c

sun.o: sun.c

types.o: types.c

clean: 
	rm -f sundial main.o drawing.o sun.o types.o util.o