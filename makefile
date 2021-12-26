CC=g++
CFLAGS=

SRC_DIR=src
OBJ_DIR=obj
DIST=dist

include .env

DEPS=$(wildcard $(SRC_DIR)/*.hpp)
OBJS=$(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(wildcard $(SRC_DIR)/*.cpp))

$(info ${DEPS})
$(info ${OBJS})

LIBS=-fopenmp -lboost_program_options

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp $(DEPS) $(OBJ_DIR)
	$(CC) -c -o $@ $< $(CFLAGS) $(LIBS)

$(DIST)/${DIST_NAME}: $(OBJS) $(DIST)
	$(CC) -o $@ $(OBJS) $(LIBS)

$(DIST):
	mkdir -p $@

$(OBJ_DIR):
	mkdir -p $@

.PHONY: clean

clean: $(OBJ_DIR)
	rm -rf $<

