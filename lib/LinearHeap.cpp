#include "LinearHeap.h"

LinearHeap::LinearHeap(int _n) {
	n = _n;
	po = -1;
	
	head = new int[n];

	pre = new int[n];
	next = new int[n];
	key = new int[n];

	for(int i = 0;i < n;i ++) {
		head[i] = -1;
		key[i] = 0;
	}
}

LinearHeap::~LinearHeap() {
	if(n) {
		delete[] head;
		delete[] pre;
		delete[] next;
		delete[] key;
	}
}

void LinearHeap::insert(int id, int value) {
#ifdef _DEBUG_
	if(value > n) {
		printf("Key value %d is too large..\n", value);
		return ;
	}
#endif

	key[id] = value;
	pre[id] = -1;
	next[id] = head[value];
	if(head[value] != -1) pre[head[value]] = id;
	head[value] = id;

	//printf("Linked Lists::insert(id %d, value %d)\n", id, value);

	if(value > po) po = value;
}

void LinearHeap::get_max(int &id, int &value) {
	while(po >= 0&&head[po] == -1) -- po;

#ifdef _DEBUG_
	if(po < 0) {
		printf("Heap is empty...\n");
		return ;
	}
#endif

	id = head[po];
	value = key[id];
}

int LinearHeap::extract_max(int &id, int &value) {
	while(po >= 0&&head[po] == -1) -- po;

	if(po < 0) {
		//printf("Heap is empty...\n");
		return 0;
	}

	id = head[po];
	value = key[id];

	head[po] = next[id];
	if(head[po] != -1) pre[head[po]] = -1;

	//printf("Linked Lists::extract_max(id %d, value %d)\n", id, value);

	return 1;
}

void LinearHeap::remove(int id) {
	if(pre[id] == -1) {
#ifdef _DEBUG_
		if(head[key[id]] != id) {
			printf("There is something wroing in LinkedLists::remove...\n");
			return ;
		}
#endif

		head[key[id]] = next[id];
		if(next[id] != -1) pre[next[id]] = -1;
	}
	else {
		int pid = pre[id];
		next[pid] = next[id];
		if(next[id] != -1) pre[next[id]] = pid;
	}
}

void LinearHeap::update(int id, int value) {
	remove(id);
	insert(id, value);
}

void LinearHeap::clear() {
	for(int i = 0;i <= po;i ++) head[i] = -1;

	po = -1;
}

