#pragma once

#include "Core/Core.hpp"
#include "DataBuffer.hpp"

WARP_TYPE_NAME_2(Data, DataBufferVector);

namespace Warp::Data {

	class DataBufferVector {
	public:
		DataBufferVector();

		DataBufferVector(uint64_t init_memory_size, uint64_t alignment = 4);

		~DataBufferVector() = default;

		DataBufferVector(const DataBufferVector&) = delete;

		DataBufferVector & operator=(const DataBufferVector&) = delete;

		DataBufferVector(DataBufferVector&& other) noexcept = default;

		DataBufferVector& operator=(DataBufferVector&& other) noexcept = default;

		bool push_back(const void* p, uint64_t size);

		template<class T> bool push_back(const T& element) {
			return push_back(&element, sizeof(T));
		}

		bool push_back_empty(uint64_t size);

		template<class T> bool fetch_back(T& element) {
			return fetch_back(&element, sizeof(T));
		}

		bool move(uint64_t source_offset, uint64_t target_offset, uint64_t size) const;

		bool pop_back(uint64_t size);

		bool fetch_back(void* p, uint64_t size);

		template<class T> bool read_at(uint64_t address_offset, T* p) {
			return read_at(address_offset, p, sizeof(T));
		}

		bool read_at(uint64_t address_offset, void* p, uint64_t size) const;

		template<class T> T* at(uint64_t address_offset) {
			return static_cast<T*>(at(address_offset));
		}

		void* at(uint64_t address_offset) const;

		inline void clear() {
			_offset = 0;
		}

		inline void clear_with_memory() {
			_offset = 0;
			_buffer.clear_with_memory();
		}

		inline void reserve(uint64_t size) {
			if (total_size() < size) { _buffer.resize(size); }
		}

		inline void resize(uint64_t size) {
			_buffer.resize(size);
			_offset = size;
		}

		inline void* data() const { return _buffer.data(); }

		inline uint64_t size() const { return _offset; }

		inline uint64_t alignment() const { return _buffer.aligment(); }

		inline uint64_t total_size() const { return _buffer.size(); }

		inline bool is_address_vailed(uint64_t absolute_address) const {
			return absolute_address < (uint64_t)data() + size() && absolute_address >= (uint64_t)data();
		}

	public:
		uint64_t _offset{ 0 };
		DataBuffer _buffer{};
	};
}