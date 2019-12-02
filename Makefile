INCLUDE := -I.
OPFLAGS := -O2
CFLAGS := -Wall -Wextra -std=c++17 $(INCLUDE) $(OPFLAGS)
LDFLAGS := 

all: optimize

optimize: optimize.o Chromosome.o Loss.o Pool.o
	g++ $^ $(CFLAGS) $(LDFLAGS) -o $@

%.o: %.cc
	g++ -c $+ $(CFLAGS) -o $@

.PHONY: clean
clean:
	rm -f *.o
