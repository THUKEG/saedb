/*
 * Heap.h
 *
 *  Created on: Apr 1, 2013
 *      Author: zhangjing0544
 */

#ifndef HEAP_H_
#define HEAP_H_

#define INIT_ARRAY_SIZE 65535

#include <map>

namespace std {

class Heap {
	pair<double, int>* ranked_array; // first element is the value to be ranked, second element is the original index
	map<int, int> index_map; // first element is the original index, second element is the ranked index

	void max_heap_delete_by_ranked_index(int r_index);
	void max_heap_delete(int index);
	void max_heap_adjust(int r_index);
	void swap(pair<double, int>& a, pair<double, int>& b);

public:

	int heap_cap_size; //heap capacity
	int heap_size;

	Heap();
	virtual ~Heap();


	void push(int index, double value);
	void modify(int index, double value);
	pair<double, int> top();
	void pop();
	void print_heap();
};

} /* namespace std */
#endif /* HEAP_H_ */
