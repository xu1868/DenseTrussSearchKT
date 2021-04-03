CC = g++
CXXFLAGS =  -O3 -ggdb -std=c++17
LIBS = -lrt -fopenmp -lSnap
INC = /usr/local/include/snap-core /usr/local/include/glib-core /usr/local/include/snap-adv /usr/local/include/snap-exp
INC_PARAMS = $(foreach d, $(INC), -I$d)
OBJECTS = Utility.o LinearHeap.o local_search.o mynet.o

all: $(OBJECTS)

$(OBJECTS): %.o : %.cpp
	$(CC) -c $(CXXFLAGS) $(INC_PARAMS) -o $@ $< $(LIBS) 

clean:
	rm -f *.o main