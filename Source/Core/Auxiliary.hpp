#pragma once

#include "Collection.hpp"
#include "Allocator.hpp"
#include "Data/DataUnion.hpp"

namespace Warp {

	class Auxiliary {
	public:

		void* operator new(size_t size) {
			return mi_malloc(size);
		}

		void operator delete(void* ptr) {
			mi_free(ptr);
		}

		void set_value(const MString& key, std::string_view text) {
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
					_data.emplace(key, (uint64_t)val);
				}
			} else if constexpr (std::is_integral_v<T> && std::is_signed_v<T>) {
				if (_data.contains(key)) {
					_data[key].set_i64(val);
				} else {
					_data.emplace(key, (int64_t)val);
				}
			}else if constexpr (std::is_floating_point_v<T>) {
				if (_data.contains(key)) {
					_data[key].set_double(val);
				} else {
					_data.emplace(key, (double)val);
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

		void set_data(const MString& key, const void* p, uint32_t size) {
			if (_data.contains(key)) {
				_data[key].set_data(p, size);
			} else {
				_data[key] = std::move(Data::DataUnion(p, size));
			}
		}

		template<class T> std::optional<T> get_value(const MString& key) {
			if (_data.contains(key)) {
				if constexpr (std::is_same_v<T, std::string_view>) {
					return _data[key].get_text();
				} else if constexpr (std::is_same_v<T, MString>) {
					return MString(_data[key].get_text());
				} else if constexpr (std::is_integral_v<T> && std::is_unsigned_v<T>) {
					return (T)(_data[key].get_u64());
				} else if constexpr (std::is_integral_v<T> && std::is_signed_v<T>) {
					return (T)(_data[key].get_i64());
				} else if constexpr (std::is_floating_point_v<T>) {
					return (T)(_data[key].get_double());
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

		bool get_data(const MString& key, void* p, uint32_t size) {
			if (_data.contains(key)) {
				return _data[key].get_data(p, size);
			}
			return false;
		}

		void remove_value(const MString& key) {
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
	private:
		MMap<MString, Data::DataUnion> _data{};
	};
}