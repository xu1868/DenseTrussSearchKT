#ifndef _HEAP_H_
#define _HEAP_H_

class Heap {
public:
	virtual void insert(int id, int value) = 0;
	virtual void remove(int id) = 0;
	virtual void get_max(int &id, int &value) = 0;
	virtual int extract_max(int &id, int &value) = 0;
	virtual bool exist(int id) = 0;
	virtual int get_key(int id) = 0;
	virtual void set_key(int id, int k) = 0;
	virtual void update(int id, int value) = 0;
};

#endif