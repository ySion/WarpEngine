#include "DataTypes.hpp"
#include "Core/Collection.hpp"

using namespace Warp::Data;

uint8_t Warp::Data::data_packet_basic_type_aligned(const DataPacketMemberType type) {
	switch (type) {
	case DataPacketMemberType::Boolean:
		return 1;
	case DataPacketMemberType::Char: case DataPacketMemberType::I8:
		return 1;
	case DataPacketMemberType::I16:
		return 2;
	case DataPacketMemberType::I32:
		return 4;
	case DataPacketMemberType::I64:
		return 8;
	case DataPacketMemberType::U8:
		return 1;
	case DataPacketMemberType::U16:
		return 2;
	case DataPacketMemberType::U32:
		return 4;
	case DataPacketMemberType::U64:
		return 8;
	case DataPacketMemberType::Float:
		return 4;
	case DataPacketMemberType::Double:
		return 8;
	case DataPacketMemberType::Vec2:    case DataPacketMemberType::Vec2I32: case DataPacketMemberType::Vec2U32:
		return 8;
	case DataPacketMemberType::Vec3:    case DataPacketMemberType::Vec3I32:	case DataPacketMemberType::Vec3U32:
		return 16;
	case DataPacketMemberType::Vec4:    case DataPacketMemberType::Vec4I32: case DataPacketMemberType::Vec4U32:
		return 16;
	case DataPacketMemberType::Mat2x2:  case DataPacketMemberType::Mat2x2I32:
		return 16;
	case DataPacketMemberType::Mat3x3:  case DataPacketMemberType::Mat3x3I32:
		return 16;
	case DataPacketMemberType::Mat4x4:  case DataPacketMemberType::Mat4x4I32:
		return 16;
	case DataPacketMemberType::Vec2D:   case DataPacketMemberType::Vec2I64: case DataPacketMemberType::Vec2U64:
		return 16;
	case DataPacketMemberType::Vec3D:   case DataPacketMemberType::Vec3I64:	case DataPacketMemberType::Vec3U64:
		return 32;
	case DataPacketMemberType::Vec4D:   case DataPacketMemberType::Vec4I64:	case DataPacketMemberType::Vec4U64:
		return 32;
	case DataPacketMemberType::Mat2x2D: case DataPacketMemberType::Mat2x2I64:
		return 32;
	case DataPacketMemberType::Mat3x3D: case DataPacketMemberType::Mat3x3I64:
		return 32;
	case DataPacketMemberType::Mat4x4D: case DataPacketMemberType::Mat4x4I64:
		return 32;
	case DataPacketMemberType::Document:  case DataPacketMemberType::Ptr:
		return 8;
	case DataPacketMemberType::Structure:
		return 0;
	default:
		return 0;
	}
}

uint8_t Warp::Data::data_packet_basic_type_size_eval_aligned(const DataPacketMemberType type) {
	switch (type) {
	case DataPacketMemberType::Boolean:
		return 1;
	case DataPacketMemberType::Char: case DataPacketMemberType::I8:
		return 1;
	case DataPacketMemberType::I16:
		return 2;
	case DataPacketMemberType::I32:
		return 4;
	case DataPacketMemberType::I64:
		return 8;
	case DataPacketMemberType::U8:
		return 1;
	case DataPacketMemberType::U16:
		return 2;
	case DataPacketMemberType::U32:
		return 4;
	case DataPacketMemberType::U64:
		return 8;
	case DataPacketMemberType::Float:
		return 4;
	case DataPacketMemberType::Double:
		return 8;
	case DataPacketMemberType::Vec2:     case DataPacketMemberType::Vec2I32:  case DataPacketMemberType::Vec2U32:
		return 8;
	case DataPacketMemberType::Vec3:     case DataPacketMemberType::Vec3I32:  case DataPacketMemberType::Vec3U32:
		return 12;
	case DataPacketMemberType::Vec4:     case DataPacketMemberType::Vec4I32:  case DataPacketMemberType::Vec4U32:
		return 16;
	case DataPacketMemberType::Mat2x2:   case DataPacketMemberType::Mat2x2I32:
		return 16;
	case DataPacketMemberType::Mat3x3:   case DataPacketMemberType::Mat3x3I32:
		return 48;
	case DataPacketMemberType::Mat4x4:   case DataPacketMemberType::Mat4x4I32:
		return 64;
	case DataPacketMemberType::Vec2D:    case DataPacketMemberType::Vec2I64:
		return 16;
	case DataPacketMemberType::Vec3D:    case DataPacketMemberType::Vec3I64:
		return 32;
	case DataPacketMemberType::Vec4D:    case DataPacketMemberType::Vec4I64:
		return 32;
	case DataPacketMemberType::Mat2x2D:  case DataPacketMemberType::Mat2x2I64: case DataPacketMemberType::Vec2U64:
		return 32;
	case DataPacketMemberType::Mat3x3D:  case DataPacketMemberType::Mat3x3I64: case DataPacketMemberType::Vec3U64:
		return 96;
	case DataPacketMemberType::Mat4x4D:  case DataPacketMemberType::Mat4x4I64: case DataPacketMemberType::Vec4U64:
		return 128;
	case DataPacketMemberType::Document: case DataPacketMemberType::Ptr:
		return 8;
	case DataPacketMemberType::Structure:
		return 0;
	default:
		return 0;
	}
}

DataPacketMemberType Warp::Data::data_packet_basic_type_string_to_type(std::string_view str) {
	static const MMap<std::string_view, DataPacketMemberType> mapper = {
		{"bool", DataPacketMemberType::Boolean},
		{"char", DataPacketMemberType::Char},
		{"i8", DataPacketMemberType::I8},
		{"i16", DataPacketMemberType::I16},
		{"i32", DataPacketMemberType::I32},
		{"i64", DataPacketMemberType::I64},
		{"u8", DataPacketMemberType::U8},
		{"u16", DataPacketMemberType::U16},
		{"u32", DataPacketMemberType::U32},
		{"u64", DataPacketMemberType::U64},
		{"float", DataPacketMemberType::Float},
		{"double", DataPacketMemberType::Double},

		{"vec2", DataPacketMemberType::Vec2},
		{"vec3", DataPacketMemberType::Vec3},
		{"vec4", DataPacketMemberType::Vec4},

		{"vec2i32", DataPacketMemberType::Vec2I32},
		{"vec3i32", DataPacketMemberType::Vec3I32},
		{"vec4i32", DataPacketMemberType::Vec4I32},

		{"vec2u32", DataPacketMemberType::Vec2I32},
		{"vec3u32", DataPacketMemberType::Vec3I32},
		{"vec4u32", DataPacketMemberType::Vec4I32},

		{"vec2d", DataPacketMemberType::Vec2D},
		{"vec3d", DataPacketMemberType::Vec3D},
		{"vec4d", DataPacketMemberType::Vec4D},

		{"vec2i64", DataPacketMemberType::Vec2I64},
		{"vec3i64", DataPacketMemberType::Vec3I64},
		{"vec4i64", DataPacketMemberType::Vec4I64},

		{"vec2u64", DataPacketMemberType::Vec2I64},
		{"vec3u64", DataPacketMemberType::Vec3I64},
		{"vec4u64", DataPacketMemberType::Vec4I64},

		{"mat2x2", DataPacketMemberType::Mat2x2},
		{"mat3x3", DataPacketMemberType::Mat3x3},
		{"mat4x4", DataPacketMemberType::Mat4x4},

		{"mat2x2i32", DataPacketMemberType::Mat2x2I32},
		{"mat3x3i32", DataPacketMemberType::Mat3x3I32},
		{"mat4x4i32", DataPacketMemberType::Mat4x4I32},

		{"mat2x2d", DataPacketMemberType::Mat2x2D},
		{"mat3x3d", DataPacketMemberType::Mat3x3D},
		{"mat4x4d", DataPacketMemberType::Mat4x4D},

		{"mat2x2i64", DataPacketMemberType::Mat2x2I64},
		{"mat3x3i64", DataPacketMemberType::Mat3x3I64},
		{"mat4x4i64", DataPacketMemberType::Mat4x4I64},

		{"document", DataPacketMemberType::Document},
		{"ptr", DataPacketMemberType::Ptr},
		{"struct", DataPacketMemberType::Structure}
	};
	const auto type = mapper.find(str);
	if (type == mapper.end()) {
		return DataPacketMemberType::Invaild;
	}
	return type->second;
}

std::string_view Warp::Data::data_packet_basic_type_type_to_string(DataPacketMemberType type) {
	switch (type) {
	case DataPacketMemberType::Boolean: return "bool";
	case DataPacketMemberType::Char: return "char";
	case DataPacketMemberType::I8: return "i8";
	case DataPacketMemberType::I16: return "i16";
	case DataPacketMemberType::I32: return "i32";
	case DataPacketMemberType::I64: return "i64";
	case DataPacketMemberType::U8: return "u8";
	case DataPacketMemberType::U16: return "u16";
	case DataPacketMemberType::U32: return "u32";
	case DataPacketMemberType::U64: return "u64";
	case DataPacketMemberType::Float: return "float";
	case DataPacketMemberType::Double: return "double";
	case DataPacketMemberType::Vec2: return "vec2";
	case DataPacketMemberType::Vec3: return "vec3";
	case DataPacketMemberType::Vec4: return "vec4";
	case DataPacketMemberType::Vec2I32: return "vec2i32";
	case DataPacketMemberType::Vec3I32: return "vec3i32";
	case DataPacketMemberType::Vec4I32: return "vec4i32";
	case DataPacketMemberType::Vec2U32: return "vec2U32";
	case DataPacketMemberType::Vec3U32: return "vec3U32";
	case DataPacketMemberType::Vec4U32: return "vec4U32";
	case DataPacketMemberType::Vec2D: return "vec2d";
	case DataPacketMemberType::Vec3D: return "vec3d";
	case DataPacketMemberType::Vec4D: return "vec4d";
	case DataPacketMemberType::Vec2I64: return "vec2i64";
	case DataPacketMemberType::Vec3I64: return "vec3i64";
	case DataPacketMemberType::Vec4I64: return "vec4i64";
	case DataPacketMemberType::Vec2U64: return "vec2U64";
	case DataPacketMemberType::Vec3U64: return "vec3U64";
	case DataPacketMemberType::Vec4U64: return "vec4U64";
	case DataPacketMemberType::Mat2x2: return "mat2x2";
	case DataPacketMemberType::Mat3x3: return "mat3x3";
	case DataPacketMemberType::Mat4x4: return "mat4x4";
	case DataPacketMemberType::Mat2x2I32: return "mat2x2i32";
	case DataPacketMemberType::Mat3x3I32: return "mat3x3i32";
	case DataPacketMemberType::Mat4x4I32: return "mat4x4i32";
	case DataPacketMemberType::Mat2x2D: return "mat2x2d";
	case DataPacketMemberType::Mat3x3D: return "mat3x3d";
	case DataPacketMemberType::Mat4x4D: return "mat4x4d";
	case DataPacketMemberType::Mat2x2I64: return "mat2x2i64";
	case DataPacketMemberType::Mat3x3I64: return "mat3x3i64";
	case DataPacketMemberType::Mat4x4I64: return "mat4x4i64";
	case DataPacketMemberType::Document: return "document";
	case DataPacketMemberType::Ptr: return "ptr";
	case DataPacketMemberType::Structure: return "struct";
	default: return "unknown";
	}
}
