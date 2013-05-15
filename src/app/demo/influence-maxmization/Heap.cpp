/*
 * Heap.cpp
 *
 *  Created on: Apr 1, 2013
 *      Author: zhangjing0544
 */

#include "Heap.h"

namespace std {

Heap::Heap() {
	heap_size = 0;
	heap_cap_size = INIT_ARRAY_SIZE;
	ranked_array = new pair<double, int> [heap_cap_size];
}

Heap::~Heap() {
	// TODO Auto-generated destructor stub
}

/*  swap two values*/
void Heap::swap(pair<double, int>& a, pair<double, int>& b) {
	pair<double, int> temp = make_pair(a.first, a.second);
	a.first = b.first;
	a.second = b.second;
	b.first = temp.first;
	b.second = temp.second;
}

/*
 * Delete element with original index as "index"
 * delete the element and insert the last element into the ranked index of the deleted element, and adjust the heap from the index
 */
void Heap::max_heap_delete(int index) {
	int r_index = index_map[index];
//	printf("delete index=%d, ranked index=%d\n", index, r_index);
	max_heap_delete_by_ranked_index(r_index);
}

/*
 * Delete element with ranked index as "r_index"
 * delete the element and insert the last element into the ranked index of the deleted element, and adjust the heap from the index
 */
void Heap::max_heap_delete_by_ranked_index(int r_index) {
	ranked_array[r_index].first = ranked_array[heap_size - 1].first;
	ranked_array[r_index].second = ranked_array[heap_size - 1].second;

	heap_size--;
	max_heap_adjust(r_index);

}

/*
 * Heap adjust from the index as "index"
 * select the largest one from the two children
 * return if the root is larger than the largest one
 * else swap the root with the largest one, and adjust the heap with the swapped child as root
 */
void Heap::max_heap_adjust(int r_index) {
	int left_index = (r_index << 1) + 1; // the index of the left tree with root as index ( index begins from 0)
	int right_index = (r_index << 1) + 2; // the index of the right tree with root as index ( index begins from 0)
	int largest = r_index;
	if (left_index <= (heap_size - 1)
			&& ranked_array[left_index].first > ranked_array[largest].first)
		largest = left_index;
	if (right_index <= (heap_size - 1)
			&& ranked_array[right_index].first > ranked_array[largest].first)
		largest = right_index;
	if (r_index == largest) {
		index_map[ranked_array[r_index].second] = r_index;
		return;
	} else {
		swap(ranked_array[r_index], ranked_array[largest]);
		index_map[ranked_array[r_index].second] = r_index;
		max_heap_adjust(largest);
	}
}

/*
 * Push element (index, value) into the heap
 * insert the element to the end of the heap, and then adjust it from the bottom to the up.
 */
void Heap::push(int index, double value) {
	if (heap_size + 1 > heap_cap_size) {
		ranked_array = (pair<double, int>*) (ranked_array,
				2 * INIT_ARRAY_SIZE * sizeof(pair<double, int> ));
	}

	ranked_array[heap_size] = make_pair(value, index);
//	printf("current insert element=%lf\n", ranked_array[heap_size].first);
	heap_size++;
	int r_index = heap_size - 1;
	int r_father_index = 0;
	int c_father_index = 0;
	int c_index = 0;
	while (r_index) {
//		printf("r_index=%d\n", r_index);
		r_father_index = (r_index - 1) / 2;
		if (ranked_array[r_index].first > ranked_array[r_father_index].first) {
			swap(ranked_array[r_index], ranked_array[r_father_index]);

//			printf("changed\n");
			c_index = ranked_array[r_index].second;
			index_map[c_index] = r_index;

			r_index = r_father_index;
		} else {
			index_map[index] = r_index;
			break;
		}
	}

//	print_heap();

}

/*
 * Modify the value of an element with original index as "index" to "value"
 */

void Heap::modify(int index, double value) {
	// delete
	max_heap_delete(index);
	// insert
	push(index, value);
}
/*
 * remove the top element form the heap
 */
void Heap::pop() {
	Heap::max_heap_delete_by_ranked_index(0);
}

/*
 * fetch the top element from the heap
 */
pair<double, int> Heap::top() {
	//printf("Heap top %lf(%d)\t", ranked_array[0].first, ranked_array[0].second);
	return ranked_array[0];
}

void Heap::print_heap() {
	printf("Heap:\n");
	for (int i = 0; i < heap_size; ++i)
		printf("%lf(%d)\t", ranked_array[i].first, ranked_array[i].second);
	printf("\n");
}

} /* namespace std */
