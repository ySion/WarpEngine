#pragma once

#include "Allocator.hpp"
#include "Object.hpp"

namespace Warp {

	template<class ChildClass, class ParentClass>
		requires std::is_base_of_v<Object, ParentClass>
	class Inherit : public ParentClass {
	public:
		template<typename... Args>
		Inherit(Args&&... args) : ParentClass(std::forward<Args>(args)...) {}

		virtual ~Inherit() override = default;

		static void* operator new(size_t size) {
			return Allocator<ChildClass>::get_instance()->allocate(size);
		}

		static void operator delete(void* ptr) {
			Allocator<ChildClass>::get_instance()->deallocate(ptr);
		}

		template<typename... Args>
		static std::unique_ptr<ChildClass> make(Args&&... args) {
			return std::unique_ptr<ChildClass>(new ChildClass(std::forward<Args>(args)...));
		}

		size_t get_object_size() const noexcept override{ return sizeof(ChildClass); }

		const char* get_class_name() const noexcept override { return type_name<ChildClass>(); }

		const std::type_info& get_type_info() const noexcept override { return typeid(ChildClass); }

		bool is_compatible(const std::type_info& type) const noexcept override { return typeid(ChildClass) == type || ParentClass::is_compatible(type); }

		bool is_ptr_available(void* ptr) const noexcept { return Allocator<ChildClass>::get_instance()->is_ptr_vaild(ptr); }
	};

	template<class ChildClass, class ParentClass>
		requires std::is_base_of_v<Object, ParentClass>
	class InheritWithoutManager : public ParentClass {
	public:
		template<typename... Args>
		InheritWithoutManager(Args&&... args) : ParentClass(args...) {}

		virtual ~InheritWithoutManager() override = default;

		size_t get_object_size() const noexcept override { return sizeof(ChildClass); }

		const char* get_class_name() const noexcept override { return type_name<ChildClass>(); }

		const std::type_info& get_type_info() const noexcept override { return typeid(ChildClass); }

		bool is_compatible(const std::type_info& type) const noexcept override { return typeid(ChildClass) == type || ParentClass::is_compatible(type); }
	};

	class InheritPure {
	public:
		InheritPure() = default;

		virtual ~InheritPure() = default;

		InheritPure(const InheritPure&) = delete;

		InheritPure operator=(const InheritPure&) = delete;

		void* operator new(size_t size) { return mi_malloc(size);}

		void operator delete(void* ptr) { return mi_free(ptr); }
	};

	class InheritPureCopy {
	public:
		InheritPureCopy() = default;

		virtual ~InheritPureCopy() = default;

		void* operator new(size_t size) { return mi_malloc(size); }

		void operator delete(void* ptr) { return mi_free(ptr); }
	};
}
