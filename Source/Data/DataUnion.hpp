#pragma once

#include "DataTypes.hpp"

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
		DataUnion();

		explicit DataUnion(const DataUnion& other);

		explicit DataUnion(DataUnion&& other) noexcept;

		DataUnion& operator=(const DataUnion& other);

		DataUnion& operator=(DataUnion&& other) noexcept;

		DataUnion& operator=(std::string_view str);

		DataUnion(std::string_view str);

		DataUnion(const void* p, uint32_t size);

		DataUnion(uint64_t u64);

		DataUnion(int64_t i64);

		DataUnion(double d64);

		~DataUnion();

		template<DataTraitSignedInt T>
		DataUnion& operator=(T v) { set_i64(v); return *this; }

		template<DataTraitUnsignedInt T>
		DataUnion& operator=(uint64_t v) { set_u64(v); return *this; }

		template<DataTraitFloating T>
		DataUnion& operator=(T v) { set_double(v); return *this; }

		DataPacketMemberType get_type() const;

		void set_i64(int64_t i64);

		void set_u64(uint64_t u64);

		void set_double(double d64);

		void set_text(std::string_view str);

		void set_data(const void* data, uint32_t size);

		void reset();

		bool is_vailed() const;

		uint64_t get_u64() const;

		double get_double() const;

		int64_t get_i64() const;

		std::string_view get_text() const;

		bool get_data(void* data, uint32_t size) const;

		std::pair<uint8_t*, uint32_t> get_data_ptr() const;
	};
}
