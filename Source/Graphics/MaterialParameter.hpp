#pragma once
#include <vector>
#include "Data/Data.hpp"

namespace Warp::Gpu {

	class MaterialSource;

	enum class MaterialParameterType : uint8_t {
		Invaild		= (uint8_t)Data::DataPacketMemberType::Invaild,
		Boolean		= (uint8_t)Data::DataPacketMemberType::Boolean,
		Integer		= (uint8_t)Data::DataPacketMemberType::I32,
		Float		= (uint8_t)Data::DataPacketMemberType::Float,
		Double		= (uint8_t)Data::DataPacketMemberType::Double,
		Vec2		= (uint8_t)Data::DataPacketMemberType::Vec2,
		Vec4		= (uint8_t)Data::DataPacketMemberType::Vec4,
		Vec2I32		= (uint8_t)Data::DataPacketMemberType::Vec2I32,
		Vec4I32		= (uint8_t)Data::DataPacketMemberType::Vec4I32,
		Vec2U32		= (uint8_t)Data::DataPacketMemberType::Vec2U32,
		Vec4U32		= (uint8_t)Data::DataPacketMemberType::Vec4U32,
		Color		= (uint8_t)Data::DataPacketMemberType::Vec4,
		Mat4x4		= (uint8_t)Data::DataPacketMemberType::Mat4x4,
		Texture		= (uint8_t)Data::DataPacketMemberType::I32,
		TextureCube	= (uint8_t)Data::DataPacketMemberType::I32,
	};

	inline Data::DataPacketMemberType material_type_map_to_data_packet_type(MaterialParameterType type) {
		return (Data::DataPacketMemberType)type;
	}
}
