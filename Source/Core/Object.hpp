#pragma once

#include <typeinfo>
#include <typeindex>

#include "TypeName.hpp"
#include "Auxiliary.hpp"
#include "Collection.hpp"

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

		Object& operator=(Object&& other) = delete;

		virtual size_t get_object_size() const noexcept { return sizeof(Object); }

		virtual const char* get_class_name() const noexcept { return type_name<Object>(); }

		virtual const std::type_info& get_type_info() const noexcept { return typeid(Object); }

		virtual bool is_compatible(const std::type_info& type) const noexcept { return typeid(Object) == type; }

		virtual int compare(const Object& other) const noexcept;

		template<typename T> T* cast() noexcept { return is_compatible(typeid(T)) ? static_cast<T*>(this) : nullptr; }

		Auxiliary* get_or_create_auxiliary() noexcept;

		uint64_t get_object_id() const noexcept { return _id; }

		virtual void write(Output& input) {};

		virtual void read(Input& output) {};

		//virtual void accept(Visitor* modifier);

		//virtual void traverse(Visitor* modifier) {}

		inline uint64_t get_dirty() const { return dirty; }

		inline uint64_t set_dirty() { dirty += 1; return dirty; }

		template<class T> requires std::is_arithmetic_v<T>
		void set_value(const MString& name, T val) { get_or_create_auxiliary()->set_value(name, val); }

		void set_value(const MString& name, std::string_view text) { get_or_create_auxiliary()->set_value(name, text); }

		inline void set_value_data(const MString& name, void* p, uint32_t size) { get_or_create_auxiliary()->set_data(name, p, size); }
		
		template<class T>
		void set_value_data(const MString& name, const T& structure_data) { get_or_create_auxiliary()->set_data(name, &structure_data, sizeof(T)); }

		inline void  remove_value(const MString& key) { get_or_create_auxiliary()->remove_value(key);}

		template<class T>
		std::optional<T> get_value(const MString& name) { return get_or_create_auxiliary()->get_value<T>(name); }

		inline bool get_value_data(const MString& name, void* p, uint32_t size) { return get_or_create_auxiliary()->get_data(name, p, size); }

		template<class T>
		bool get_value_data(const MString& name, T& p) { return get_or_create_auxiliary()->get_data<T>(name, p); }

		template<class T>
		auto get_value_data_ref(const MString& name) { return get_or_create_auxiliary()->get_data_ref<T>(name); }

		void* operator new(size_t size);

		void operator delete(void* ptr);
	private:
		std::unique_ptr<Auxiliary> _auxiliary{};

		uint64_t _id;

		uint64_t dirty = 0;
	};
}
