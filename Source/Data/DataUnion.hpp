#pragma once

#include "DataTypes.hpp"
#include "Core/Collection.hpp"
#include "Core/Logger.hpp"

namespace Warp::Data {

	class DataUnion {
		union {
			uint64_t _u64;
			int64_t _i64;
			double _d64;
			uint8_t* _text;
			uint8_t* _data_ptr;
		} _data;
		DataPacketMemberType _type;
		uint32_t _data_size;
	public:
		inline DataUnion() : _type(DataPacketMemberType::Invaild), _data_size(0) {}

		explicit DataUnion(const DataUnion& other);

		explicit DataUnion(DataUnion&& other) noexcept;

		DataUnion& operator=(const DataUnion& other);

		DataUnion& operator=(DataUnion&& other) noexcept;

		DataUnion(std::string_view str);

		DataUnion(const void* p, uint32_t size);

		inline DataUnion(uint64_t u64) : _type(DataPacketMemberType::U64), _data_size(0) { _data._u64 = u64; }

		inline DataUnion(int64_t i64) : _type(DataPacketMemberType::I64), _data_size(0) { _data._i64 = i64; }

		inline DataUnion(double d64) : _type(DataPacketMemberType::Double), _data_size(0) { _data._u64 = d64; }

		~DataUnion();

		inline DataPacketMemberType get_type() const { return _type; }

		void set_i64(int64_t i64);

		void set_u64(uint64_t u64);

		void set_double(double d64);

		void set_text(std::string_view str);

		void set_data(const void* data, uint32_t size);

		inline uint64_t get_u64() const {
			if (_type == DataPacketMemberType::U64) {
				return _data._u64;
			}
			MString msg = MString::format("DataUnion::get_u64: type not match, need type: {}, current type: {}",
				data_packet_basic_type_type_to_string(DataPacketMemberType::U64), data_packet_basic_type_type_to_string(_type));
			error(msg);
			return 0;
		}

		inline double get_double() const {
			if (_type == DataPacketMemberType::Double) {
				return _data._d64;
			}
			MString msg = MString::format("DataUnion::get_double: type not match, need type: {}, current type: {}",
				data_packet_basic_type_type_to_string(DataPacketMemberType::Double), data_packet_basic_type_type_to_string(_type));
			error(msg);
			return 0;
		}

		inline int64_t get_i64() const {
			if (_type == DataPacketMemberType::I64) {
				return _data._i64;
			}
			MString msg = MString::format("DataUnion::get_i64: type not match, need type: {}, current type: {}",
				data_packet_basic_type_type_to_string(DataPacketMemberType::I64), data_packet_basic_type_type_to_string(_type));
			error(msg);
			return 0;
		}

		inline std::string_view get_text() const {
			return _type == DataPacketMemberType::Text ? std::string_view(reinterpret_cast<const char*>(_data._text)) : std::string_view();
		}

		inline bool get_data(void* data, uint32_t size) const {
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

		inline std::pair<uint8_t*, uint32_t> get_data_ptr() const {
			if (_type == DataPacketMemberType::Ptr) {
				return { _data._data_ptr, _data_size };
			}
			return { nullptr, 0 };
		}
	};
}
