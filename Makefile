CC = g++
CXXFLAGS =  -O3 -ggdb -std=c++17
LIBS = -lrt -fopenmp -lSnap
INC = /usr/local/include/snap-core /usr/local/include/glib-core /usr/local/include/snap-adv /usr/local/include/snap-exp
INC_PARAMS = $(foreach d, $(INC), -I$d)

all: main

main: main.cpp DenseTrussSearchTopDown.o DenseTrussSearchTopDownKT.o KTIndex.o MinDenseTruss.o TrussDecomposition.o Utility.o lib/local_search.o
	$(CC) $(CXXFLAGS) -o $@ $^ $(INC_PARAMS) $(LIBS)

DenseTrussSearchTopDown.o: DenseTrussSearchTopDown.cpp TrussDecomposition.o
	$(CC) -c $(CXXFLAGS) -o $@ $^ $(INC_PARAMS) $(LIBS)

DenseTrussSearchTopDownKT.o: DenseTrussSearchTopDownKT.cpp KTIndex.o TrussDecomposition.o
	$(CC) -c $(CXXFLAGS) -o $@ $^ $(INC_PARAMS) $(LIBS)

KTIndex.o: KTIndex.cpp TrussDecomposition.o
	$(CC) -c $(CXXFLAGS) -o $@ $^ $(INC_PARAMS) $(LIBS)

MinDenseTruss.o: MinDenseTruss.cpp Utility.o TrussDecomposition.o
	$(CC) -c $(CXXFLAGS) -o $@ $^ ./lib/local_search.o $(INC_PARAMS) $(LIBS)

TrussDecomposition.o: TrussDecomposition.cpp Utility.o
	$(CC) -c $(CXXFLAGS) -o $@ $^ $(INC_PARAMS) $(LIBS)

Utility.o: Utility.cpp
	$(CC) -c $(CXXFLAGS) -o $@ $^ $(INC_PARAMS) $(LIBS)

./lib/local_search.o:
	$(MAKE) -C lib all