#include <stdlib.h>
#include <string.h>
#include <stdio.h>


/*-------HEAP---- */
unsigned int heap[2500];	// The pre-allocated heap buffer 
void *p_heap;				// Pointer to the next available memory byte from the heap
void *p_heap_start;			// Start of the heap
void *p_heap_end;			// End of the heap
void *p_heap_max;			// Maximum memory ever required 

//////////////////////////////////////////////////////////////////////////
// Simple memory allocation from a fixed buffer
//
//////////////////////////////////////////////////////////////////////////
void *osAlloc(unsigned int req_mem_size)
{
	void *ret = p_heap;
	void *p_new_heap = (void *)((unsigned int)p_heap + req_mem_size);
	if(p_heap_end < p_new_heap) 
		return 0;
	p_heap_max = (p_heap > p_heap_max) ? p_heap : p_heap_max;
	p_heap = p_new_heap;
	return ret;
}
//////////////////////////////////////////////////////////////////////////
//  Free the memory - check for the proper deallocation sequence 
//////////////////////////////////////////////////////////////////////////
void osFree(void *p_mem)
{
	if((p_mem > p_heap) || (p_mem < p_heap_start)) {
		// The memory is freed in a wrong order - break and report error
		return;
	}
	p_heap = p_mem;
}

void osHeapInit()
{
	p_heap_start = &heap[0];
	p_heap_end = &heap[0] + (sizeof(heap) * sizeof(unsigned int));
	p_heap = p_heap_start;
	p_heap_max = p_heap;
}