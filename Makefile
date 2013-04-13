CC=gcc
CFLAGS= -O3
CILK=g++
CILKFLAGS= -Werror -O3 -g -fcilkplus -lcilkrts 
LDFLAGS= -L$(CURDIR)
AR=ar

all: main

main : main.cpp vector_reducer.h Makefile
	$(CILK) $(CILKFLAGS) $@.cpp $(LDFLAGS) -o $@

clean :
	rm -f main *~
