CC=g++
CFLAGS=-I/usr/include/librsvg-2.0 -I/usr/include/glib-2.0 -I/usr/lib64/glib-2.0/include -I/usr/include/libmount -I/usr/include/blkid -I/usr/include/gdk-pixbuf-2.0 -I/usr/include/libpng16 -pthread -I/usr/include/cairo -I/usr/include/pixman-1 -I/usr/include/freetype2

SRC_DIR=src
OBJ_DIR=obj
DIST=dist

include .env

DEPS=$(wildcard $(SRC_DIR)/*.hpp)
OBJS=$(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(wildcard $(SRC_DIR)/*.cpp))

$(info ${DEPS})
$(info ${OBJS})

LIBS=-fopenmp -lboost_program_options -lrsvg-2 -lm -lgio-2.0 -lgdk_pixbuf-2.0 -lgobject-2.0 -lglib-2.0 -lcairo

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp $(DEPS) $(OBJ_DIR)
	$(CC) -c -o $@ $< $(CFLAGS) $(LIBS)

$(DIST)/${DIST_NAME}: $(OBJS) $(DIST)
	$(CC) -o $@ $(OBJS) $(LIBS) $(pkg-config --libs librsvg-2.0)

$(DIST):
	mkdir -p $@

$(OBJ_DIR):
	mkdir -p $@

.PHONY: clean

clean: $(OBJ_DIR)
	rm -rf $<

