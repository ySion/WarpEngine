#pragma once
#include <ranges>

#include "Collection.hpp"

#include "mimalloc.h"
#include "tbb/spin_rw_mutex.h"
#include "TypeName.hpp"
#include "glm/glm.hpp"

namespace Warp {

	class Object;

	class AllocatorBase;
	template<class T> class Allocator;

	struct AllocatorGlobalObjectIdCounter {

		static uint64_t get(){
			return _id_counter;
		}
	private:
		static uint64_t add() {
			_id_counter++;
			return _id_counter;
		}

		static inline uint64_t _id_counter{ 0 };

		friend class AllocatorBase;
		template<class T> friend class Allocator;
	};

	class AllocatorGroup {
		friend class AllocatorBase;
		template<class T> friend class Allocator;

		tbb::spin_rw_mutex _mutex{};

		void add_allocator(const char* name, AllocatorBase* allocator) {
			tbb::spin_rw_mutex::scoped_lock lock(_mutex, true);
			_allocators_map.insert({ name, allocator });
		}

	public:
		
		phmap::flat_hash_map<std::string_view, AllocatorBase*> _allocators_map{};

		AllocatorGroup() = default;

		~AllocatorGroup() = default;

		AllocatorGroup(const AllocatorGroup&) = delete;

		AllocatorGroup& operator=(const AllocatorGroup&) = delete;

		AllocatorGroup(AllocatorGroup&&) = delete;

		AllocatorGroup& operator=(AllocatorGroup&&) = delete;

		static AllocatorGroup* get_instance() {
			static AllocatorGroup instance;
			return &instance;
		}

		template<class T>
		Allocator<T>* get_allocator() {
			tbb::spin_rw_mutex::scoped_lock lock(_mutex, false);
			if(_allocators_map.contains(type_name<T>())) {
				return Allocator<T>::get_instance();
			}
			return nullptr;
		}

		AllocatorBase* get_allocator(std::string_view name) {
			tbb::spin_rw_mutex::scoped_lock lock(_mutex, false);
			if (_allocators_map.contains(name)) {
				return _allocators_map[name];
			}
			return nullptr;
		}

		MVector<std::pair<std::string_view, AllocatorBase*>> get_all_allocators() {
			tbb::spin_rw_mutex::scoped_lock lock(_mutex, false);
			MVector<std::pair<std::string_view, AllocatorBase*>> allocators;
			for (auto& values : _allocators_map) {
				allocators.emplace_back(values);
			}
			return allocators;
		}

		void* operator new(size_t size) { return mi_malloc(size); }

		void operator delete(void* ptr) { mi_free(ptr); }
	};


	class AllocatorBase {
	public:
		AllocatorBase() = default;

		virtual ~AllocatorBase() = default;

		AllocatorBase(const AllocatorBase&) = delete;

		AllocatorBase& operator=(const AllocatorBase&) = delete;

		AllocatorBase(AllocatorBase&&) = delete;

		AllocatorBase& operator=(AllocatorBase&&) = delete;

		virtual const char* get_object_name() = 0;

		virtual void* allocate(size_t size) = 0;

		virtual void deallocate(void* ptr) = 0;

		virtual bool is_object_vaild(void* ptr, uint64_t id) const = 0;

		virtual bool is_ptr_vaild(void* ptr) const = 0;

		virtual uint64_t get_object_count() const = 0;

		virtual uint64_t get_object_memory_use() const = 0;

		virtual uint64_t get_id_from_ptr(void* ptr) const = 0;

		void* operator new(size_t size) {
			return mi_malloc(size);
		}

		void operator delete(void* ptr) {
			mi_free(ptr);
		}
	};

	template<class T>
	class Allocator: public AllocatorBase {

		mutable tbb::spin_rw_mutex _mutex {};
	
		MMap<void*, uint64_t> _allocations_map {};

	public:
		Allocator() {
			AllocatorGroup::get_instance()->add_allocator(Allocator<T>::get_object_name(), this);
			_allocations_map.reserve(32);
		}
	
		~Allocator() override {
			tbb::spin_rw_mutex::scoped_lock lock(_mutex, true);
			for (const auto& key : _allocations_map | std::views::keys) {
				mi_free(key);
			}
		}

		constexpr const char* get_object_name() override { return type_name<T>(); }

		void* allocate(size_t size) override {
			tbb::spin_rw_mutex::scoped_lock lock(_mutex, true);
			void* ptr = mi_malloc(size);
			uint64_t id = AllocatorGlobalObjectIdCounter::add();
			_allocations_map.insert({ ptr ,id });
			return ptr;
		}

		void deallocate(void* ptr) override {
			tbb::spin_rw_mutex::scoped_lock lock(_mutex, true);
			mi_free(ptr);
			_allocations_map.erase(ptr);
		}

		constexpr static auto* get_instance() {
			static Allocator<T> instance;
			return &instance;
		}

		bool is_object_vaild(void* ptr, uint64_t id)const override {
			tbb::spin_rw_mutex::scoped_lock lock(_mutex, false);
			if(const auto res = _allocations_map.find(ptr); res != _allocations_map.end()){
				if(res->second == id){
					return true;
				}
			}
			return false;
		}

		bool is_ptr_vaild(void* ptr) const override {
			tbb::spin_rw_mutex::scoped_lock lock(_mutex, false);
			return _allocations_map.contains(ptr);
		}


		uint64_t get_object_count() const override {
			return _allocations_map.size();
		}

		uint64_t get_object_memory_use() const override  {
			return _allocations_map.size() * sizeof(T);
		}

		uint64_t get_id_from_ptr(void* ptr) const override {
			uint64_t id = -1;
			tbb::spin_rw_mutex::scoped_lock lock(_mutex, false);
			if (const auto res = _allocations_map.find(ptr); res != _allocations_map.end()) {
				id = res->second;
			}
			return id;
		}
	};
}