CC=g++
CFLAGS=
DEPS=

# Ojects
OBJ=./src/main.o

DIST=dist

LIBS=-fopenmp


%.o : %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

$(DIST)/export: $(OBJ) $(DIST)
	$(CC) -o $@ $< $(LIBS)

$(DIST):
	mkdir -p $@

