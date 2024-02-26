#include "DataUnion.hpp"

#include "Core/Core.hpp"

using namespace Warp::Data;

DataUnion::DataUnion() : _type(DataPacketMemberType::Invaild), _data_size(0) {}

DataUnion::DataUnion(const DataUnion& other) : _type(other._type), _data_size(other._data_size) {
	switch (_type) {
	case DataPacketMemberType::U64: case DataPacketMemberType::I64: case DataPacketMemberType::Double:
		_data._u64 = other._data._u64;
		break;
	case DataPacketMemberType::Document:
	{
		std::string_view text((const char*)(other._data._text));
		_data_size = static_cast<uint32_t>(text.size() + 1);
		_data._text = static_cast<uint8_t*>(mi_malloc(_data_size));
		memcpy(_data._text, other._data._text, _data_size);
		break;
	}
	case DataPacketMemberType::Ptr:
		_data._data_ptr = static_cast<uint8_t*>(mi_malloc(other._data_size));
		memcpy(_data._data_ptr, other._data._data_ptr, _data_size);
		_data_size = other._data_size;
		break;
	default:
		break;
	}
}

DataUnion::DataUnion(DataUnion&& other) noexcept : _type(other._type), _data_size(other._data_size) {
	switch (_type) {
	case DataPacketMemberType::U64: case DataPacketMemberType::I64: case DataPacketMemberType::Double:
		_data._u64 = other._data._u64;
		other._data._u64 = 0;
		break;
	case DataPacketMemberType::Document: case DataPacketMemberType::Ptr:
		_data._data_ptr = other._data._data_ptr;
		_data_size = other._data_size;
		other._data._text = nullptr;
		other._data_size = 0;
		break;
	default:
		break;
	}
	other._type = DataPacketMemberType::Invaild;
}

DataUnion& DataUnion::operator=(const DataUnion& other) {
	if (this == &other) return *this;
	if (_type == DataPacketMemberType::Document || _type == DataPacketMemberType::Ptr) {
		mi_free(_data._text);
	}

	_type = other._type;

	switch (_type) {
	case DataPacketMemberType::U64: case DataPacketMemberType::I64: case DataPacketMemberType::Double:
		_data._u64 = other._data._u64;
		_data_size = 0;
		break;
	case DataPacketMemberType::Document:
	{
		std::string_view text((const char*)(other._data._text));
		_data_size = static_cast<uint32_t>(text.size() + 1);
		_data._text = static_cast<uint8_t*>(mi_malloc(_data_size));
		memcpy(_data._text, other._data._text, _data_size);
		break;
	}
	case DataPacketMemberType::Ptr:
		_data._data_ptr = static_cast<uint8_t*>(mi_malloc(other._data_size));
		memcpy(_data._data_ptr, other._data._data_ptr, _data_size);
		_data_size = other._data_size;
		break;
	default:
		break;
	}
	return *this;
}

DataUnion& DataUnion::operator=(DataUnion&& other) noexcept {
	if (this == &other) return *this;
	if (_type == DataPacketMemberType::Document || _type == DataPacketMemberType::Ptr) {
		mi_free(_data._text);
	}

	_type = other._type;

	switch (_type) {
	case DataPacketMemberType::U64: case DataPacketMemberType::I64: case DataPacketMemberType::Double:
		_data._u64 = other._data._u64;
		break;
	case DataPacketMemberType::Document: case DataPacketMemberType::Ptr:
		_data._data_ptr = other._data._data_ptr;
		_data_size = other._data_size;
		other._data._data_ptr = nullptr;
		other._data_size = 0;
		break;
	default:
		break;
	}
	other._type = DataPacketMemberType::Invaild;
	return *this;
}

DataUnion& DataUnion::operator=(std::string_view str) {
	set_text(str); return *this;
}

DataUnion::DataUnion(std::string_view str) : _type(DataPacketMemberType::Document), _data_size(static_cast<uint32_t>(str.size() + 1)) {
	_data._text = static_cast<uint8_t*>(mi_malloc(_data_size));
	memcpy(_data._text, str.data(), _data_size);
}

DataUnion::DataUnion(const void* p, uint32_t size) {
	_data._data_ptr = static_cast<uint8_t*>(mi_malloc(size));
	memcpy(_data._data_ptr, p, size);
	_data_size = size;
	_type = DataPacketMemberType::Ptr;
}

DataUnion::DataUnion(uint64_t u64) : _type(DataPacketMemberType::U64), _data_size(0) {
	_data._u64 = u64;
}

DataUnion::DataUnion(int64_t i64) : _type(DataPacketMemberType::I64), _data_size(0) {
	_data._i64 = i64;
}

DataUnion::DataUnion(double d64) : _type(DataPacketMemberType::Double), _data_size(0) {
	_data._u64 = d64;
}

DataUnion::~DataUnion() {
	if (_type == DataPacketMemberType::Document || _type == DataPacketMemberType::Ptr) { mi_free(_data._text); }
}

DataPacketMemberType DataUnion::get_type() const {
	return _type;
}

void DataUnion::set_i64(int64_t i64) {
	if (_type == DataPacketMemberType::Document || _type == DataPacketMemberType::Ptr) { mi_free(_data._text); }
	_type = DataPacketMemberType::I64;
	_data._i64 = i64;
	_data_size = 0;
}

void DataUnion::set_u64(uint64_t u64) {
	if (_type == DataPacketMemberType::Document || _type == DataPacketMemberType::Ptr) { mi_free(_data._text); }
	_type = DataPacketMemberType::U64;
	_data._u64 = u64;
	_data_size = 0;
}

void DataUnion::set_double(double d64) {
	if (_type == DataPacketMemberType::Document || _type == DataPacketMemberType::Ptr) { mi_free(_data._text); }
	_type = DataPacketMemberType::Double;
	_data._d64 = d64;
	_data_size = 0;
}

void DataUnion::set_text(std::string_view str) {
	const uint32_t need_data_size = static_cast<uint32_t>(str.size() + 1);

	if (_type == DataPacketMemberType::Document || _type == DataPacketMemberType::Ptr) {
		if (need_data_size > _data_size) {
			mi_free(_data._text);
			_data._text = static_cast<uint8_t*>(mi_malloc(need_data_size));
			_data_size = need_data_size;
		}
	} else {
		_data._text = static_cast<uint8_t*>(mi_malloc(need_data_size));
		_data_size = need_data_size;
	}
	memcpy(_data._text, str.data(), _data_size);
	_type = DataPacketMemberType::Document;
}

void DataUnion::set_data(const void* data, uint32_t size) {
	if (_type == DataPacketMemberType::Document || _type == DataPacketMemberType::Ptr) {
		if (_data_size != size) {
			mi_free(_data._data_ptr);
			_data_size = size;
			_data._data_ptr = static_cast<uint8_t*>(mi_malloc(_data_size));
		}
	} else {
		_data_size = size;
		_data._data_ptr = static_cast<uint8_t*>(mi_malloc(_data_size));
	}

	memcpy(_data._data_ptr, data, _data_size);
	_type = DataPacketMemberType::Ptr;
}

void DataUnion::reset() {
	if (_type == DataPacketMemberType::Document || _type == DataPacketMemberType::Ptr) { mi_free(_data._text); }
	_type = DataPacketMemberType::Invaild;
}

bool DataUnion::is_vailed() const {
	return _type != DataPacketMemberType::Invaild;
}

uint64_t DataUnion::get_u64() const {
	if (_type == DataPacketMemberType::U64) {
		return _data._u64;
	}
	MString msg = MString::format("DataUnion::get_u64: type not match, need type: {}, current type: {}",
				      data_packet_basic_type_type_to_string(DataPacketMemberType::U64), data_packet_basic_type_type_to_string(_type));
	error(msg);
	return 0;
}

double DataUnion::get_double() const {
	if (_type == DataPacketMemberType::Double) {
		return _data._d64;
	}
	MString msg = MString::format("DataUnion::get_double: type not match, need type: {}, current type: {}",
				      data_packet_basic_type_type_to_string(DataPacketMemberType::Double), data_packet_basic_type_type_to_string(_type));
	error(msg);
	return 0;
}

int64_t DataUnion::get_i64() const {
	if (_type == DataPacketMemberType::I64) {
		return _data._i64;
	}
	MString msg = MString::format("DataUnion::get_i64: type not match, need type: {}, current type: {}",
				      data_packet_basic_type_type_to_string(DataPacketMemberType::I64), data_packet_basic_type_type_to_string(_type));
	error(msg);
	return 0;
}

std::string_view DataUnion::get_text() const {
	if(_type == DataPacketMemberType::Document) {
		return std::string_view((const char*)(_data._text));
	}
	return {};
}

bool DataUnion::get_data(void* data, uint32_t size) const {
	if (data && _type == DataPacketMemberType::Ptr && _data._data_ptr) {
		if (size > _data_size) {
			MString msg = MString::format("DataUnion::get_data: size not match, need size: {}, data size: {}", size, _data_size);
			error(msg);
			return false;
		}
		memcpy(data, _data._data_ptr, size);
	}
	return true;
}

std::pair<uint8_t*, uint32_t> DataUnion::get_data_ptr() const {
	if (_type == DataPacketMemberType::Ptr) {
		return { _data._data_ptr, _data_size };
	}
	return { nullptr, 0 };
}
