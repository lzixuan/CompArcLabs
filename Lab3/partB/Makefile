CC=g++

all: sim

sim: Simulation.o cache.o
	$(CC) -o $@ $^

Simulation: cache.h

cache.o: cache.h def.h

.PHONY: clean

clean:
	rm -rf sim *.o
