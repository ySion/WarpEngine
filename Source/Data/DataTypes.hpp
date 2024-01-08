#pragma once
#include <cstdint>
#include <string_view>

namespace Warp::Data {

	enum class DataPacketMemberType : uint8_t {
		Invaild = 0,
		Boolean,
		I8,
		I16,
		I32,
		I64,
		U8,
		U16,
		U32,
		U64,
		Float,
		Double,

		Vec2,
		Vec3,
		Vec4,

		Vec2I32,
		Vec3I32,
		Vec4I32,

		Vec2D,
		Vec3D,
		Vec4D,

		Vec2I64,
		Vec3I64,
		Vec4I64,

		Mat2x2,
		Mat3x3,
		Mat4x4,

		Mat2x2I32,
		Mat3x3I32,
		Mat4x4I32,

		Mat2x2D,
		Mat3x3D,
		Mat4x4D,

		Mat2x2I64,
		Mat3x3I64,
		Mat4x4I64,

		Text,   //ptr
		Ptr,	//ptr
		Structure,
	};

	uint8_t data_packet_basic_type_aligned(const DataPacketMemberType type);

	uint8_t data_packet_basic_type_size_eval_aligned(const DataPacketMemberType type);

	DataPacketMemberType data_packet_basic_type_string_to_type(std::string_view str);

	std::string_view data_packet_basic_type_type_to_string(DataPacketMemberType type);
}
