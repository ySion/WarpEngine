#include "mimalloc.h"

#include "Object.hpp"
#include "Allocator.hpp"
#include "Visitor.hpp"

Warp::Object::Object(): _id(AllocatorGlobalObjectIdCounter::get()) {}

void* Warp::Object::operator new(size_t size) {
	return Allocator<Object>::get_instance()->allocate(size);
}

void Warp::Object::operator delete(void* ptr) {
	Allocator<Object>::get_instance()->deallocate(ptr);
}

int Warp::Object::compare(const Object& other) const noexcept
{
	const std::type_index this_tpye = std::type_index(typeid(Object));
	const std::type_index other_type = std::type_index(typeid(other));
	if (this_tpye > other_type) return 1;
	if (this_tpye < other_type) return -1;
	return 0;
}

Warp::Auxiliary* Warp::Object::get_or_create_auxiliary() noexcept {
	if(!_auxiliary) _auxiliary = std::make_unique<Auxiliary>();
	return _auxiliary.get();
}