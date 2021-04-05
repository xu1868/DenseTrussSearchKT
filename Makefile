CC = g++
CXXFLAGS =  -ggdb -std=c++17
LIBS = -lrt -fopenmp -lSnap
INC = /usr/local/include/snap-core /usr/local/include/glib-core /usr/local/include/snap-adv /usr/local/include/snap-exp
INC_PARAMS = $(foreach d, $(INC), -I$d)


all: main

main: main.cpp DenseTrussSearchTopDown.o DenseTrussSearchTopDownKT.o KTIndex.o MinDenseTruss.o TrussDecomposition.o Utility.o BanksAlgorithm.o
	$(CC) $(CXXFLAGS) -o $@ $^ $(INC_PARAMS) $(LIBS)

DenseTrussSearchTopDown.o: DenseTrussSearchTopDown.cpp
	$(CC) -c $(CXXFLAGS) -o $@ $< $(INC_PARAMS)

DenseTrussSearchTopDownKT.o: DenseTrussSearchTopDownKT.cpp
	$(CC) -c $(CXXFLAGS) -o $@ $< $(INC_PARAMS)

KTIndex.o: KTIndex.cpp
	$(CC) -c $(CXXFLAGS) -o $@ $< $(INC_PARAMS)

MinDenseTruss.o: MinDenseTruss.cpp
	$(CC) -c $(CXXFLAGS) -o $@ $< $(INC_PARAMS)

TrussDecomposition.o: TrussDecomposition.cpp
	$(CC) -c $(CXXFLAGS) -o $@ $< $(INC_PARAMS)

Utility.o: Utility.cpp
	$(CC) -c $(CXXFLAGS) -o $@ $< $(INC_PARAMS)

BanksAlgorithm.o: BanksAlgorithm.cpp
	$(CC) -c $(CXXFLAGS) -o $@ $< $(INC_PARAMS)

clean:
	-rm main *.o