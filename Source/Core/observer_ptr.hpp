#pragma once

#include "Core/Allocator.hpp"

namespace Warp {

	template<typename T> class observer_ptr {
	public:
		observer_ptr() : _ptr(nullptr), _id(-1) {}

		observer_ptr(T* ptr) {
			if (!is_ptr_vaild(ptr)) {
				_ptr = nullptr;
				_id = -1;
				return;
			}

			_ptr = ptr;
			_id = ptr->get_object_id();

			if (_id == static_cast<uint64_t>(-1)) {
				_ptr = nullptr;
				_id = -1;
			}
		}

		observer_ptr(const observer_ptr& other) : _ptr(other._ptr), _id(other._id) {}

		observer_ptr(observer_ptr&& other) noexcept : _ptr(other._ptr), _id(other._id){
			other._ptr = nullptr;
			other._id = -1;
		}

		observer_ptr& operator=(const observer_ptr& other) {
			_ptr = other._ptr;
			_id = other._id;
			return *this;
		}

		observer_ptr& operator=(observer_ptr&& other) noexcept {
			_ptr = other._ptr;
			_id = other._id;
			return *this;
		}

		observer_ptr& operator=(std::nullptr_t) noexcept {
			_ptr = nullptr;
			_id = -1;
			return *this;
		}

		bool is_ptr_vaild() const noexcept {
			if (_ptr) { return Allocator<T>::get_instance()->is_ptr_vaild(_ptr); }
			return false;
		}

		bool is_object_vaild() const noexcept {
			if (_ptr && _id != static_cast<uint64_t>(-1)) { return Allocator<T>::get_instance()->is_object_vaild(_ptr, _id); }
			return false;
		}

		operator bool() const noexcept {
			return _ptr != nullptr && _id != static_cast<uint64_t>(-1);
		}

		operator T*() const noexcept {
			return _ptr;
		}

		T* get() const { return _ptr; }
		T* operator->() const { return _ptr; }
		T& operator*() const { return *_ptr; }

		auto operator<=>(const observer_ptr&) const noexcept = default;

		static inline bool is_ptr_vaild(T* ptr) noexcept {
			if (ptr) { return Allocator<T>::get_instance()->is_ptr_vaild(ptr); }
			return false;
		}

		static inline bool is_object_vaild(T* ptr, uint64_t id) noexcept {
			if (ptr && id != static_cast<uint64_t>(-1)) { return Allocator<T>::get_instance()->is_object_vaild(ptr, id); }
			return false;
		}

	private:
		T* _ptr;
		uint64_t _id;
		template<typename U> friend class observer_ptr;
	};
}