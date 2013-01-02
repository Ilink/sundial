CC=clang
BUILD_DIR = ../build
CFLAGS = -lncurses -lm
# CFLAGS = -lncurses -I/usr/share/GeoIP -llibGeoIP -lm
# CFLAGS = -I/usr/include/ncurses -lncurses -I/include -L/lib -I/usr/share/GeoIP -llibGeoIP -g -lm -v
# CFLAGS = -I/usr/include/ncurses -lncurses -I/usr/include -L/usr/lib -I/usr/share/GeoIP -llibGeoIP -g -lm -v
# CFLAGS = -I/usr/local/include/ncurses -lncurses

sundial: main.o drawing.o sun.o types.o util.o
	$(CC) -o $(BUILD_DIR)/sundial main.o drawing.o sun.o types.o util.o $(CFLAGS)

main.o: main.c

util.o: util.c

drawing.o: drawing.c

sun.o: sun.c

types.o: types.c

# get_loc.o: get_loc.c

clean: 
	rm -f sundial main.o drawing.o sun.o types.o util.o get_loc.o