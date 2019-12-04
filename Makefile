INCLUDE := -I.
OPFLAGS := -O2
CFLAGS := -Wall -Wextra -std=c++17 $(INCLUDE) $(OPFLAGS)
LDFLAGS := 

all: annealing optimize

annealing: annealing.o Chromosome.o Loss.o
	g++ $^ $(CFLAGS) $(LDFLAGS) -o $@

optimize: optimize.o Chromosome.o Loss.o Pool.o
	g++ $^ $(CFLAGS) $(LDFLAGS) -o $@

%.o: %.cc
	g++ -c $+ $(CFLAGS) -o $@

.PHONY: clean
clean:
	rm -f *.o
