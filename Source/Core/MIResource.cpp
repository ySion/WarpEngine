#include "MIResource.h"

#include "mimalloc.h"

void* MIResource::operator new(size_t size) {
	return mi_malloc(size);
}

void MIResource::operator delete(void* ptr) {
	mi_free(ptr);
}