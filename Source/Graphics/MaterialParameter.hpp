#pragma once
#include <vector>
#include "Data/Data.hpp"

namespace Warp::Gpu {

	class MaterialSource;

	enum class MaterialParamterType : uint8_t {
		Unknown = 0,
		Boolean,
		Integer,
		Float,
		Double,
		Vec2,
		Vec4,
		Color,
		Texture,
		Texture3D,
		TextureCube,
		Mat3x3,
		Mat4x4
	};

	inline Data::DataPacketMemberType material_type_map_to_data_packet_type(MaterialParamterType type) {
		switch (type) {
		case MaterialParamterType::Boolean:
			return Data::DataPacketMemberType::Boolean;
		case MaterialParamterType::Integer:
			return Data::DataPacketMemberType::I32;
		case MaterialParamterType::Float:
			return Data::DataPacketMemberType::Float;
		case MaterialParamterType::Double:
			return Data::DataPacketMemberType::Double;
		case MaterialParamterType::Vec2:
			return Data::DataPacketMemberType::Vec2;
		case MaterialParamterType::Vec4:
			return Data::DataPacketMemberType::Vec4;
		case MaterialParamterType::Color:
			return Data::DataPacketMemberType::Vec4;
		case MaterialParamterType::Mat3x3:
			return Data::DataPacketMemberType::Mat3x3;
		case MaterialParamterType::Mat4x4:
			return Data::DataPacketMemberType::Mat4x4;
		default:
			return Data::DataPacketMemberType::Invaild;
		}
	}

	struct MaterialParameter {
		bool _is_override_parent = false;
		MaterialParamterType _var_type = MaterialParamterType::Unknown;
	};

}
