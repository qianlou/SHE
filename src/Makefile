CC=g++
CCFLAGS=--std=c++11 -fopenmp
#CCFLAGS= -fopenmp
#LDFLAGS=-ltfhe-spqlios-avx
LDFLAGS=-ltfhe-spqlios-fma -ltfhe-spqlios-avx
all: test

metrics.o: metrics.cpp metrics.hpp
	$(CC) $(CCFLAGS) -c metrics.cpp

io.o: io.cpp
	$(CC) $(CCFLAGS) -c io.cpp

matrix.o: matrix.cpp matrix.hpp alu.o
	$(CC) $(CCFLAGS) -c matrix.cpp alu.cpp $(LDFLAGS)

alu.o: alu.cpp alu.hpp omp_constants.hpp
	$(CC) $(CCFLAGS) -c alu.cpp $(LDFLAGS)

encryption.o: encryption.hpp
	$(CC) $(CCFLAGS) -o encryption.o -c encryption.hpp $(LDFLAGS)

shiftNet: shiftNet.o encryption.o alu.o matrix.o logistic.o io.o metrics.o
	$(CC) $(CCFLAGS) -o shiftNet shiftNet.o alu.o matrix.o  io.o metrics.o $(LDFLAGS)

clean:
	rm -f test
	rm -f *.o *.gch




