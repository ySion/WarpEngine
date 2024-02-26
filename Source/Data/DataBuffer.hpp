#pragma once

#include "Core/Core.hpp"

WARP_TYPE_NAME_2(Data, DataBuffer);

WARP_TYPE_NAME_2(Data, DataBufferShrinkBuffer);

namespace Warp::Data {

	/// 不分配内存, 后续可以通过resize来分配
	/// 默认4字节对齐
	class DataBuffer {
	public:

		DataBuffer(uint64_t aligment = 4);

		DataBuffer(uint64_t size, int aligment = 4);

		DataBuffer(DataBuffer&& other) noexcept;

		DataBuffer& operator=(DataBuffer&& other) noexcept;

		virtual ~DataBuffer();

		void clear_with_memory();

		void resize(uint64_t new_size);

		inline bool is_vailed() const { return _data != nullptr; }

		inline uint64_t size() const { return _size; }

		inline uint64_t aligment() const { return _aligment; }

		inline uint8_t* data() const { return _data; }

	private:
		uint8_t* _data{};
		uint64_t _size{ 0 };
		uint64_t _aligment{ 4 };
	};

	class DataBufferShrinkBuffer : public DataBuffer {
	public:
		DataBufferShrinkBuffer() = default;

		~DataBufferShrinkBuffer() override = default;

		DataBufferShrinkBuffer(DataBufferShrinkBuffer&& other) noexcept = default;

		DataBufferShrinkBuffer& operator=(DataBufferShrinkBuffer&& other) noexcept = default;

		void* write_buffer(const void* p, uint64_t size);
	};

}