CC=clang
BUILD_DIR = ../build
CFLAGS = -lm

sundial: main.o sun.o types.o
	$(CC) -o $(BUILD_DIR)/sundial main.o sun.o types.o $(CFLAGS)

main.o: main.c

sun.o: sun.c

types.o: types.c

clean: 
	rm -f sundial main.o sun.o types.o util.o