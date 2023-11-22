#pragma once

class MIResource {
public:

	void* operator new(size_t size);

	void operator delete(void* ptr);
};