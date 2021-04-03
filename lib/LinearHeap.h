#ifndef _LINEAR_HEAP_H_
#define _LINEAR_HEAP_H_

//#include "Utility.h"
#include "Heap.h"

class LinearHeap: public Heap {
private:
	int n;
	int po; //the potential maximum key
	int *head;
	int *pre;
	int *next;
	int *key;

public:
	LinearHeap(int _n) ;
	~LinearHeap();

	void insert(int id, int value) ;
	int get_key(int id) { return key[id]; }
	void set_key(int id, int value) { key[id] = value; }
	bool exist(int id) { return key[id] != 0; }
	void get_max(int &id, int &value) ;
	int extract_max(int &id, int &value) ;
	void remove(int id) ;
	void update(int id, int value) ;
	void clear();
};

#endif