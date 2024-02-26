#include "DataBufferVector.hpp"

using namespace Warp::Data;

DataBufferVector::DataBufferVector() : _buffer(64, 4) {
}

DataBufferVector::DataBufferVector(uint64_t init_memory_size, uint64_t alignment) : _buffer(init_memory_size, alignment) {

}

//DataBufferVector::DataBufferVector(DataBufferVector&& other) noexcept {
//	_offset = other._offset;
//	_buffer = std::move(other._buffer);
//	other._offset = 0;
//}
//
//DataBufferVector& DataBufferVector::operator=(DataBufferVector&& other) noexcept
//{
//	_offset = other._offset;
//	_buffer = std::move(other._buffer);
//	other._offset = 0;
//	return *this;
//}

bool DataBufferVector::push_back(const void* p, uint64_t size) {
	if (p == nullptr) return false;
	if (size == 0) return true;

	try {
		if (total_size() < _offset + size) {

			if (static_cast<uint64_t>(1.5 * total_size()) >= _offset + size) {
				_buffer.resize(static_cast<uint64_t>(1.5 * total_size()));
			} else {
				_buffer.resize(total_size() + _offset + size);
			}
		}

	} catch (Exception e) {
		return false;
	}

	memcpy(_buffer.data() + _offset, p, size);

	_offset += size;
	return true;
}

bool DataBufferVector::push_back_empty(uint64_t size) {
	if (size == 0) return true;

	try {
		if (total_size() < _offset + size) {

			if (static_cast<uint64_t>(1.5 * total_size()) >= _offset + size) {
				_buffer.resize(static_cast<uint64_t>(1.5 * total_size()));
			} else {
				_buffer.resize(total_size() + _offset + size);
			}
		}

	} catch (Exception e) {
		return false;
	}

	_offset += size;
	return true;
}

bool DataBufferVector::move(uint64_t source_offset, uint64_t target_offset, uint64_t size) const {
	if (source_offset + size > _offset) { return false; }
	if (target_offset + size > _offset) { return false; }
	if (source_offset == target_offset) { return true; }
	memcpy(_buffer.data() + target_offset, _buffer.data() + source_offset, size);
	return true;
}

bool DataBufferVector::pop_back(uint64_t size) {
	if (size > _offset) {
		_offset = 0;
		return false;
	}
	_offset -= size;
	return true;
}

bool DataBufferVector::fetch_back(void* p, uint64_t size) {
	if (p == nullptr) return false;

	if (_offset < size) { return false; }

	_offset -= size;
	memcpy(p, _buffer.data() + _offset, size);

	return true;
}

bool DataBufferVector::read_at(uint64_t address_offset, void* p, uint64_t size) const {
	if (p == nullptr) return false;
	if (address_offset + size > _offset) return false;

	memcpy(p, _buffer.data() + address_offset, size);
	return true;
}

void* DataBufferVector::at(uint64_t address_offset) const {

	if(!is_address_vailed((uint64_t)_buffer.data() + address_offset)) {
		warn("buffer address access: {} ~ {}, but {}, current size {}.", (uint64_t)data(), (uint64_t)data() + size(), address_offset, size());
		return nullptr;
	}
	return _buffer.data() + address_offset;
}
