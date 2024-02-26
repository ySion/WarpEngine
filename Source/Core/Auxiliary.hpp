#pragma once

#include "Collection.hpp"
#include "Allocator.hpp"
#include "Data/DataUnion.hpp"

namespace Warp {

	class Auxiliary {
	public:

		inline void* operator new(size_t size) {
			return mi_malloc(size);
		}

		inline void operator delete(void* ptr) {
			mi_free(ptr);
		}

		inline void set_value(const MString& key, std::string_view text) {
			if (_data.contains(key)) {
				_data[key].set_text(text);
			} else {
				_data.emplace(key, text);
			}
		}

		template<class T> requires std::is_arithmetic_v<T>
		void set_value(const MString& key, T val) {
			if constexpr (std::is_integral_v<T> && std::is_unsigned_v<T>) {
				if (_data.contains(key)) {
					_data[key].set_u64(val);
				} else {
					_data.emplace(key, static_cast<uint64_t>(val));
				}
			} else if constexpr (std::is_integral_v<T> && std::is_signed_v<T>) {
				if (_data.contains(key)) {
					_data[key].set_i64(val);
				} else {
					_data.emplace(key, static_cast<int64_t>(val));
				}
			}else if constexpr (std::is_floating_point_v<T>) {
				if (_data.contains(key)) {
					_data[key].set_double(val);
				} else {
					_data.emplace(key, static_cast<double>(val));
				}
			}
		}

		template<class T> void set_data(const MString& key, const T& value) {
			if (_data.contains(key)) {
				_data[key].set_data(&value, sizeof(T));
			} else {
				_data[key] = std::move(Data::DataUnion(&value, sizeof(T)));
			}
		}

		inline void set_data(const MString& key, const void* p, uint32_t size) {
			if (_data.contains(key)) {
				_data[key].set_data(p, size);
			} else {
				_data[key] = std::move(Data::DataUnion(p, size));
			}
		}

		inline bool is_value_exist(const MString& key) const {
			return _data.contains(key);
		}

		template<class T> std::optional<T> get_value(const MString& key) {
			if (_data.contains(key)) {
				if constexpr (std::is_same_v<T, std::string_view>) {
					return _data[key].get_text();
				} else if constexpr (std::is_same_v<T, MString>) {
					return MString(_data[key].get_text());
				} else if constexpr (std::is_integral_v<T> && std::is_unsigned_v<T>) {
					return static_cast<T>(_data[key].get_u64());
				} else if constexpr (std::is_integral_v<T> && std::is_signed_v<T>) {
					return static_cast<T>(_data[key].get_i64());
				} else if constexpr (std::is_floating_point_v<T>) {
					return static_cast<T>(_data[key].get_double());
				}
			}
			return std::nullopt;
		}

		template<class T> bool get_data(const MString& key, T& value) {
			if (_data.contains(key)) {
				return _data[key].get_data(&value, sizeof(T));
			}
			return false;
		}

		inline bool get_data(const MString& key, void* p, uint32_t size) {
			if (_data.contains(key)) {
				return _data[key].get_data(p, size);
			}
			return false;
		}

		inline void remove_value(const MString& key) {
			if(_data.contains(key)) {
				_data.erase(key);
			}
		}

		template<class T = void>
		std::pair<T*, uint32_t> get_data_ref(const MString& key) {
			if (_data.contains(key)) {
				auto temp = _data[key].get_data_ptr();
				return { static_cast<T*>(temp.first), temp.second};
			}
			return { nullptr, 0 };
		}

		inline uint64_t new_version() {
			_version += 1;
			return _version;
		}

		inline uint64_t get_current_version() const{
			return _version;
		}
	private:
		MMap<MString, Data::DataUnion> _data{};
		uint64_t _version = 0;
	};
}