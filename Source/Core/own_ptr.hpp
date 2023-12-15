#pragma once
#include "Allocator.hpp"

namespace Warp {

	template<typename T>
	class own_ptr {
	public:
		own_ptr() noexcept = default;

		~own_ptr() noexcept { if (is_object_vaild()) { delete _ptr; } }

		own_ptr(T* ptr) noexcept {
			if(!is_ptr_vaild(ptr)) {
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

		own_ptr(const own_ptr& other) = delete;

		own_ptr(own_ptr&& other) noexcept : _ptr(other._ptr), _id(other._id) { other._ptr = nullptr; other._id = -1; }

		//copy op
		own_ptr& operator=(const own_ptr& other) = delete;

		//move op
		own_ptr& operator=(own_ptr&& other) noexcept {
			if (_ptr != other._ptr) {
				if (is_object_vaild()) { delete _ptr; }
				_ptr = other._ptr;
				_id = other._id;
				other._ptr = nullptr;
				other._id = -1;
			}
			return *this;
		}

		auto operator<=>(const own_ptr&) const noexcept = default;

		T* get() const noexcept { return _ptr; }

		T* operator->() const noexcept { return _ptr; }

		T& operator*() const noexcept { return *_ptr; }

		T* release() noexcept {
			T* temp = _ptr;
			_ptr = nullptr;
			_id = -1;
			return temp;
		}

		bool is_ptr_vaild() const noexcept {
			if (_ptr) { return Allocator<T>::get_instance()->is_ptr_vaild(_ptr); }
			return false;
		}

		bool is_object_vaild() const noexcept {
			if (_ptr && _id != static_cast<uint64_t>(-1)) { return Allocator<T>::get_instance()->is_object_vaild(_ptr, _id); }
			return false;
		}

		static bool is_ptr_vaild(T* ptr) noexcept {
			if (ptr) { return Allocator<T>::get_instance()->is_ptr_vaild(ptr); }
			return false;
		}

		static bool is_object_vaild(T* ptr, uint64_t id) noexcept {
			if (ptr && id != static_cast<uint64_t>(-1)) { return Allocator<T>::get_instance()->is_object_vaild(ptr, id); }
			return false;
		}

		operator bool() const noexcept {
			return _ptr != nullptr && _id != static_cast<uint64_t>(-1);
		}

		void reset(T* ptr = nullptr) noexcept {
			if (is_object_vaild()) { delete _ptr; }
			_ptr = ptr;

			if (ptr) {
				const uint64_t id = Allocator<T>::get_instance()->get_id_from_ptr(_ptr);
				if (id != static_cast<uint64_t>(-1)) {
					_id = id;
				} else {
					_ptr = nullptr;
					_id = -1;
				}
			}
		}

		void reset(T* ptr, uint64_t id) noexcept {
			if (is_object_vaild()) { delete _ptr; }
			_ptr = ptr;
			_id = id;
		}

		void swap(own_ptr& other) noexcept {
			T* temp = _ptr;
			_ptr = other._ptr;
			other._ptr = temp;
		}

	private:
		template<typename U> friend class ref_tpr;
		template<typename U2> friend class observer_ptr;
		T* _ptr{ nullptr };
		uint64_t _id = -1;
	};
}
