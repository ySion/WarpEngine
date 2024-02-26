#include "DataBuffer.hpp"

using namespace Warp::Data;

DataBuffer::DataBuffer(uint64_t aligment) :_size(0), _aligment(aligment) {}

DataBuffer::DataBuffer(uint64_t size, int aligment) : _size(size), _aligment(aligment) {
	_data = static_cast<uint8_t*>(mi_malloc_aligned(size, _aligment));

	if (_data == nullptr) {
		MString msg = MString::format("DataBuffer::DataBuffer: failed to allocate memory, size: {}, aligment: {}", _size, _aligment);
		error(msg);
		throw Exception{ msg, 0 };
	}
}

DataBuffer::DataBuffer(DataBuffer&& other) noexcept {
	_data = other._data;
	_size = other._size;
	_aligment = other._aligment;
	other._data = nullptr;
	other._size = 0;
}

DataBuffer& DataBuffer::operator=(DataBuffer&& other) noexcept {
	_data = other._data;
	_size = other._size;
	_aligment = other._aligment;
	other._data = nullptr;
	other._size = 0;
	return *this;
}

DataBuffer::~DataBuffer() {
	if (_data != nullptr) {
		mi_free_aligned(_data, _aligment);
	}
}

void DataBuffer::clear_with_memory() {
	if (_data != nullptr) { mi_free_aligned(_data, _aligment); }
	_data = nullptr;
	_size = 0;
}

void DataBuffer::resize(uint64_t new_size) {
	if (_data == nullptr) {
		_data = static_cast<uint8_t*>(mi_malloc_aligned(new_size, _aligment));
	} else {
		_data = static_cast<uint8_t*>(mi_realloc_aligned(_data, new_size, _aligment));
	}

	if (_data == nullptr) {
		MString msg = MString::format("DataBuffer::DataBuffer: failed to resize memory, new size: {}, new aligment: {}", _size, _aligment);
		error(msg);
		throw Exception{ msg, 0 };
	}

	_size = new_size;
}

void* DataBufferShrinkBuffer::write_buffer(const void* p, uint64_t buffer_size) {
	if (size() != buffer_size) {
		resize(buffer_size);
	}

	memcpy(data(), p, buffer_size);
	return data();
}
