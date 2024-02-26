#pragma once

#include "Core/Core.hpp"
#include "DataBufferVector.hpp"

WARP_TYPE_NAME_2(Data, DataBufferUnorderedUniformVector);

namespace Warp::Data {

	/// 此类允许延迟设置element_size, 使用init可以延迟调用, 如果不设置, 默认元素大小为1, 请在使用前确保element_size已经设置
	/// 删除不保证顺序
	class DataBufferUnorderedUniformVector {
	public:

		DataBufferUnorderedUniformVector() = default;

		~DataBufferUnorderedUniformVector() = default;

		DataBufferUnorderedUniformVector(uint64_t element_size_byte, uint64_t reserve_count = 0, uint64_t alignment = 4) : _element_size(element_size_byte), _vector(element_size_byte * reserve_count, alignment) {}

		DataBufferUnorderedUniformVector(const DataBufferUnorderedUniformVector& other) = delete;

		DataBufferUnorderedUniformVector& operator=(const DataBufferUnorderedUniformVector& other) = delete;

		DataBufferUnorderedUniformVector(DataBufferUnorderedUniformVector&& other) noexcept {
			_element_size = other._element_size;
			_vector = std::move(other._vector);
			other._element_size = 0;
		}

		DataBufferUnorderedUniformVector& operator=(DataBufferUnorderedUniformVector&& other) noexcept {
			_element_size = other._element_size;
			_vector = std::move(other._vector);
			other._element_size = 0;
			return *this;
		}

		void init(uint64_t element_size_byte) {
			_vector.clear_with_memory();
			_element_size = element_size_byte;
		}

		template<class T> bool push_back(const T& element) {
			if (sizeof(T) != _element_size) {
				MString msg = MString::format("DataBufferUnorderedUniformVector::push_back: element size not match, element size: {}, uniform element size: {}", sizeof(T), _element_size);
				error(msg);
				return false;
			}

			return _vector.push_back(&element, _element_size);
		}

		inline bool push_back(const void* p) {
			return _vector.push_back(p, _element_size);
		}

		//移动覆盖
		inline bool move(uint64_t index_1, uint64_t index_2) const {
			if(index_1 == index_2) { return true; }
			if(is_index_vailed(index_1) || is_index_vailed(index_2)) { return false; }
			return _vector.move(index_1 * _element_size, index_2 * _element_size, _element_size);
		}

		inline bool push_back_empty(uint64_t element) {
			return _vector.push_back_empty(_element_size * element);
		}

		inline bool pop_back() {
			return _vector.pop_back(_element_size);
		}

		template<class T> bool fetch_back(T& element) {
			if (sizeof(T) != _element_size) {
				MString msg = MString::format("DataBufferUnorderedUniformVector::push_back: element size not match, element size: {}, uniform element size: {}", sizeof(T), _element_size);
				error(msg);
				return false;
			}
			return fetch_back(&element, sizeof(T));
		}

		inline bool fetch_back(void* p) {
			return _vector.fetch_back(p, _element_size);
		}

		inline void reserve(uint64_t element_count) {
			_vector.reserve(_element_size * element_count);
		}

		template<class T> T* at(uint64_t index) const {
			return static_cast<T*>(at(index));
		}

		inline void* at(uint64_t index) const {
			if (!is_index_vailed(index)) {
#if defined WARP_DEBUG
				MString msg = MString::format("DataBufferUnorderedUniformVector::at: index out of range, index: {}, but count: {}", index, count());
				error(msg);
#endif
				return nullptr;
			}
			return _vector.at(index * _element_size);
		}

		inline void resize(uint64_t element_count){
			_vector.resize(_element_size * element_count);
		}

		inline bool is_address_vailed(uint64_t absolute_address) const {
			return _vector.is_address_vailed(absolute_address);
		}

		inline bool is_index_vailed(uint64_t index) const { return index < count(); }

		inline void* data() const { return _vector.data(); }

		inline uint64_t count() const { return _vector.size() / _element_size;}

		inline uint64_t size_used_memory() const { return _vector.size(); }

		inline uint64_t size_total_memory() const { return _vector.total_size(); }

		inline uint64_t size_element() const { return _element_size; }
	public:
		uint64_t _element_size { 1 };
		DataBufferVector _vector { };
	};

}
