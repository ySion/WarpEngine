#pragma once
#include "TypeName.hpp"
#include "Allocator.hpp"
#include "own_ptr.hpp"
#include "Object.hpp"

namespace Warp {

	template<class ChildClass, class ParentClass>
		requires std::is_base_of_v<Object, ParentClass>
	class Inherit : public ParentClass {
	public:
		template<typename... Args>
		Inherit(Args&&... args) : ParentClass(args...) {}

		template<typename... Args>
		static own_ptr<ChildClass> create(Args&&... args) {
			return own_ptr(new ChildClass(args...));
		}

		static void* operator new(size_t size) {
			return Allocator<ChildClass>::get_instance()->allocate(size);
		}

		static void operator delete(void* ptr) {
			Allocator<ChildClass>::get_instance()->deallocate(ptr);
		}

		size_t get_object_size() const noexcept override{ return sizeof(ChildClass); }

		const char* get_class_name() const noexcept override { return type_name<ChildClass>(); }

		const std::type_info& get_type_info() const noexcept override { return typeid(ChildClass); }

		bool is_compatible(const std::type_info& type) const noexcept override { return typeid(ChildClass) == type || ParentClass::is_compatible(type); }

		bool is_ptr_available(void* ptr) const noexcept { return Allocator<ChildClass>::get_instance()->is_ptr_vaild(ptr); }
	};
}