#pragma once

#include <typeinfo>
#include <typeindex>

#include "TypeName.hpp"
#include "Auxiliary.hpp"
#include "MIStl.h"

WARP_TYPE_NAME(Object);

namespace Warp {

	class Visitor;
	class Input;
	class Output;

	class Object {
	public:

		Object();

		virtual ~Object() = default;

		Object(const Object&) = delete;

		Object& operator=(const Object&) = delete;

		Object(Object&& other) = delete;

		Object & operator=(Object && other) = delete;

		virtual size_t get_object_size() const noexcept { return sizeof(Object); }

		virtual const char* get_class_name() const noexcept { return type_name<Object>(); }

		virtual const std::type_info& get_type_info() const noexcept { return typeid(Object); }

		virtual bool is_compatible(const std::type_info& type) const noexcept { return typeid(Object) == type; }

		virtual int compare(const Object& other) const noexcept;

		template<typename T> T* cast() noexcept { return is_compatible(typeid(T)) ? static_cast<T*>(this) : nullptr; }

		Auxiliary* get_or_create_auxiliary() noexcept;

		uint64_t get_object_id() const noexcept { return _id; }

		virtual void set_value(std::string_view name, const Object& other) {}

		virtual bool get_value(std::string_view name, Object& other) const { return false; }

		virtual void write(Output& input){};

		virtual void read(Input& output){};

		virtual void accept(Visitor* modifier);

		virtual void traverse(Visitor* modifier) {}

		void* operator new(size_t size);

		void operator delete(void* ptr);
	private:
		std::unique_ptr<Auxiliary> _auxiliary {};

		uint64_t _id;
	};
}
