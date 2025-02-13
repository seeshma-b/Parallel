CC = g++
CFLAGS = -Wall -O2
OBJ = main.o nbody.o utils.o

nbody: $(OBJ)
	$(CC) $(CFLAGS) -o nbody $(OBJ)

main.o: main.cpp nbody.h
	$(CC) $(CFLAGS) -c main.cpp

nbody.o: nbody.cpp nbody.h
	$(CC) $(CFLAGS) -c nbody.cpp

utils.o: utils.cpp utils.h
	$(CC) $(CFLAGS) -c utils.cpp

clean:
	rm -f nbody $(OBJ)
