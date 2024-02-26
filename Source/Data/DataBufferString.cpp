#include "DataBufferString.hpp"

using namespace Warp::Data;

DataBufferString::DataBufferString(std::string_view str) {
	_string_buffer.resize(str.length() + 1);
	memcpy(_string_buffer.data(), str.data(), str.length() + 1);
}

DataBufferString::DataBufferString(DataBufferString&& other) noexcept {
	_string_len = other._string_len;
	_string_buffer = std::move(other._string_buffer);
	other._string_len = 0;
}

DataBufferString& DataBufferString::operator=(DataBufferString&& other) noexcept {
	_string_len = other._string_len;
	_string_buffer = std::move(other._string_buffer);
	other._string_len = 0;
	return *this;
}

const char* DataBufferString::write_string(std::string_view str) {
	if ((str.length() + 1) > _string_buffer.size()) {
		_string_buffer.resize(str.length() + 1);
	}

	if (32 + (str.length() + 1) * 4 < _string_buffer.size()) { // 收缩策略
		_string_buffer.resize(32 + str.length() * 2);
	}

	memcpy(_string_buffer.data(), str.data(), str.length() + 1);
	return reinterpret_cast<const char*>(_string_buffer.data());
}
