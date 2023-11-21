#include <stdio.h>
#include <stdlib.h> // malloc, realloc, free

#include "adt_heap.h"

/* Reestablishes heap by moving data in child up to correct location heap array
*/
static void _reheapUp( HEAP *heap, int index) {
	
	if (index != 0) {
		int parent = (index - 1) / 2;
		
		if (heap->compare(heap->heapArr[index], heap->heapArr[parent]) > 0) {
			void *temp = heap->heapArr[parent];
			heap->heapArr[parent] = heap->heapArr[index];
			heap->heapArr[index] = temp;
			index = (index - 1) / 2;
			_reheapUp(heap, index);
		}
	}
} //end of _reheapUp

/* Reestablishes heap by moving data in root down to its correct location in the heap
*/
static void _reheapDown( HEAP *heap, int index){
	
	void *leftkey;
	void *rightkey;
	int large;
	void *temp;
	
	if (index * 2 + 1 <= heap->last) {
		leftkey = heap->heapArr[index * 2 + 1];
		
		if (index * 2 + 2 <= heap->last)
			rightkey = heap->heapArr[index * 2 + 2];
		else
			rightkey = NULL;
	
		if (rightkey == NULL || heap->compare(leftkey, rightkey) >= 0)
			large = index * 2 + 1;
		else
			large = index * 2 + 2;
		
		if (heap->compare(heap->heapArr[index], heap->heapArr[large]) < 0) {
			temp = heap->heapArr[index];
			heap->heapArr[index] = heap->heapArr[large];
			heap->heapArr[large] = temp;
			_reheapDown(heap, large);
		}
	}
} //end of _reheapDown

/* Allocates memory for heap and returns address of heap head structure
if memory overflow, NULL returned
The initial capacity of the heap should be 10
*/
HEAP *heap_Create( int (*compare) (const void *arg1, const void *arg2)) {
	
	HEAP *headheap = (HEAP *)malloc(sizeof(HEAP));
	
	if (headheap == NULL)
		return NULL;
	
	headheap->last = -1;
	headheap->capacity = 10;
	headheap->heapArr = (void **)malloc(headheap->capacity * sizeof(void *));
	headheap->compare = compare;
	
	return headheap;
} //end of heap_Create

/* Free memory for heap
*/
void heap_Destroy( HEAP *heap, void (*remove_data)(void *ptr)) {
	
	void *dlt;
	
	while (heap->last >= 0) {
		dlt = heap->heapArr[heap->last];
		heap->last--;
		remove_data(dlt);
	}
		
	heap->capacity = 0;
	free(heap->heapArr);
	heap->compare = NULL;
	
	free(heap);
} //end of heap_Destroy

/* Inserts data into heap 삽입시 capacity보다 원소의 수가 많아지려고 할 때, realloc을 해야 함 (100씩 증가하도록 해야 함)
return 1 if successful; 0 if heap full
*/
int heap_Insert( HEAP *heap, void *dataPtr) {
	
	void **array;
	
	if (heap->capacity - 1 <= heap->last) {
		heap->capacity += 100;
		array = (void **)realloc(heap->heapArr, heap->capacity * sizeof(void *));
		
		if (array == NULL)
			return 0;
			
		heap->heapArr = array;
	}
	
	heap->last++;
	heap->heapArr[heap->last] = dataPtr;
	_reheapUp(heap, heap->last);
	return 1;
} //end of heap_Insert

/* Deletes root of heap and passes data back to caller
return 1 if successful; 0 if heap empty
*/
int heap_Delete( HEAP *heap, void **dataOutPtr) {
	
	if (heap->last < 0)
		return 0;
	
	*dataOutPtr = heap->heapArr[0];
	heap->heapArr[0] = heap->heapArr[heap->last];
	heap->last--;
	_reheapDown(heap, 0);
	return 1;
} //end of heap_Delete

/*
return 1 if the heap is empty; 0 if not
*/
int heap_Empty(  HEAP *heap) {
	
	if (heap->last < 0)
		return 1;
	else
		return 0;
} //end of heap_Empty

/* Print heap array */
void heap_Print( HEAP *heap, void (*print_func) (const void *data)) {
	for (int i = 0; i < heap->last + 1; i++)
		print_func(heap->heapArr[i]);
} //end of heap_Print