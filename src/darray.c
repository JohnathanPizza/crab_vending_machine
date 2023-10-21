#include "darray.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "error.h"

struct DArray newDArray(size_t elemCt, size_t elemSize){
	struct DArray da = {NULL, elemSize, 0};
	da.base = malloc(elemCt * elemSize);
	if(da.base == NULL){
		printError(ERROR_RESERVE, strerror(errno));
		exit(1);
	}
	da.allocatedCount = elemCt;
	return da;
}

void resizeDArray(struct DArray* da, size_t newCt){
	da->base = realloc(da->base, newCt * da->elementSize);
	if(da->base == NULL){
		printError(ERROR_RESERVE, strerror(errno));
		exit(1);
	}
	da->allocatedCount = newCt;
}

void freeDArray(struct DArray* da){
	free(da->base);
	da->base = NULL;
	da->allocatedCount = 0;
}

void* getDArray(struct DArray* da, size_t index){
	if(index >= da->allocatedCount){
		return NULL;
	}
	return (char*)da->base + da->elementSize * index;
}

void addDArray(struct DArray* da, void* obj, size_t index){
	void* ptr = getDArray(da, index);
	while(ptr == NULL){
		resizeDArray(da, da->allocatedCount + 50);
		ptr = getDArray(da, index);
	}
	for(int a = 0; a < da->elementSize; ++a){
		((unsigned char*)ptr)[a] = ((unsigned char*)obj)[a];
	}
}
