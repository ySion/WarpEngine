#pragma once

#include "Allocator.hpp"
#include "Object.hpp"

namespace Warp {

	template<class ChildClass, class ParentClass>
		requires std::is_base_of_v<Object, ParentClass>
	class Inherit : public ParentClass {
	public:
		template<typename... Args>
		Inherit(Args&&... args) : ParentClass(args...) {}

		virtual ~Inherit() override = default;

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
