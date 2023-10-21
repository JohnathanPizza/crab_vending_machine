// a dynamically sizing array type

#ifndef DARRAY_H
#define DARRAY_H

#include <stddef.h>

struct DArray{
	void* base;
	size_t elementSize;
	size_t allocatedCount;
};

// return a DArray with elemCt * elemSize bytes allocated already
// remember the size of each element from elemSize
struct DArray newDArray(size_t elemCt, size_t elemSize);

// resizes an existing DArray to allocate newCt objects
// uses the elementSize variable in the DArray to allocate
void resizeDArray(struct DArray* da, size_t newCt);

// frees any allocated data in the DArray and resets variables
void freeDArray(struct DArray* da);

// get a pointer to the beginning of the object at index
// based on the elementSize variable in the DArray
void* getDArray(struct DArray* da, size_t index);

// copy data at obj into the DArray at index
// resizes the DArray if needed
void addDArray(struct DArray* da, void* obj, size_t index);

#endif
