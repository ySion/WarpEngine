#include "DataPacketDataStructureDescriptor.hpp"

#include "Core/observer_ptr.hpp"
#include "Core/Exception.hpp"
#include "Core/Logger.hpp"

using namespace Warp::Data;

uint8_t Warp::Data::data_packet_basic_type_aligned(const DataPacketMemberType type) {
	switch (type) {
	case DataPacketMemberType::Boolean:
		return 1;
	case DataPacketMemberType::I8:
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
	case DataPacketMemberType::Vec2:    case DataPacketMemberType::Vec2I32:
		return 8;		    
	case DataPacketMemberType::Vec3:    case DataPacketMemberType::Vec3I32:
		return 16;		    
	case DataPacketMemberType::Vec4:    case DataPacketMemberType::Vec4I32:
		return 16;		    
	case DataPacketMemberType::Mat2x2:  case DataPacketMemberType::Mat2x2I32:
		return 16;		    
	case DataPacketMemberType::Mat3x3:  case DataPacketMemberType::Mat3x3I32:
		return 16;		    
	case DataPacketMemberType::Mat4x4:  case DataPacketMemberType::Mat4x4I32:
		return 16;		    
	case DataPacketMemberType::Vec2D:   case DataPacketMemberType::Vec2I64:
		return 16;		    
	case DataPacketMemberType::Vec3D:   case DataPacketMemberType::Vec3I64:
		return 32;		    
	case DataPacketMemberType::Vec4D:   case DataPacketMemberType::Vec4I64:
		return 32;
	case DataPacketMemberType::Mat2x2D: case DataPacketMemberType::Mat2x2I64:
		return 32;
	case DataPacketMemberType::Mat3x3D: case DataPacketMemberType::Mat3x3I64:
		return 32;
	case DataPacketMemberType::Mat4x4D: case DataPacketMemberType::Mat4x4I64:
		return 32;
	case DataPacketMemberType::Text:  case DataPacketMemberType::Ptr:
		return 8;
	case DataPacketMemberType::Structure: return 0;
	default:
		return 0;
	}
}

uint8_t Warp::Data::data_packet_basic_type_size_eval_aligned(const DataPacketMemberType type) {
	switch (type) {
	case DataPacketMemberType::Boolean:
		return 1;
	case DataPacketMemberType::I8:
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
	case DataPacketMemberType::Vec2:     case DataPacketMemberType::Vec2I32:
		return 8;		     
	case DataPacketMemberType::Vec3:     case DataPacketMemberType::Vec3I32:
		return 12;		     
	case DataPacketMemberType::Vec4:     case DataPacketMemberType::Vec4I32:
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
	case DataPacketMemberType::Mat2x2D:  case DataPacketMemberType::Mat2x2I64:
		return 32;		     
	case DataPacketMemberType::Mat3x3D:  case DataPacketMemberType::Mat3x3I64:
		return 96;		     
	case DataPacketMemberType::Mat4x4D:  case DataPacketMemberType::Mat4x4I64:
		return 128;
	case DataPacketMemberType::Text: case DataPacketMemberType::Ptr:
		return 8;
	case DataPacketMemberType::Structure:
		return 0;

	default:
		return 0; // ptr size
	}
}

DataPacketMemberType Warp::Data::data_packet_basic_type_string_to_type(std::string_view str) {
	static const MMap<std::string_view, DataPacketMemberType> mapper = {
		{"bool", DataPacketMemberType::Boolean},
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

		{"vec2d", DataPacketMemberType::Vec2D},
		{"vec3d", DataPacketMemberType::Vec3D},
		{"vec4d", DataPacketMemberType::Vec4D},

		{"vec2i64", DataPacketMemberType::Vec2I64},
		{"vec3i64", DataPacketMemberType::Vec3I64},
		{"vec4i64", DataPacketMemberType::Vec4I64},

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

		{"text", DataPacketMemberType::Text},
		{"ptr", DataPacketMemberType::Ptr},
		{"struct", DataPacketMemberType::Structure}
	};
	const auto type = mapper.find(str);
	if (type == mapper.end()) {
		return DataPacketMemberType::Invaild;
	}
	return type->second;
}


DataPacketDataStructureDescriptor::DataPacketDataStructureDescriptor(std::string_view struct_name,
	std::string_view desc_text, uint16_t alignment) {
	_name = struct_name;
	_desc_text = desc_text;
	_struct_alignment = alignment;
	_init_alignment = alignment;
}

DataPacketDataStructureDescriptor& DataPacketDataStructureDescriptor::add_normal_member(DataPacketMemberType type,
	std::string_view name, std::string_view desc_string, const MVector<uint32_t>& arr, uint16_t alignment, uint32_t custom_data) {
	if (type > DataPacketMemberType::Invaild && type < DataPacketMemberType::Structure) {

		uint64_t type_size = data_packet_basic_type_size_eval_aligned(type);

		const uint16_t type_aligment = data_packet_basic_type_aligned(type);
		const uint16_t final_type_aligment = type_aligment > alignment ? type_aligment : alignment;

		for (auto i : arr) {
			type_size *= i;
		}
		_members.emplace_back(type, type_size, calc_size_after_alignment(_struct_size, final_type_aligment), alignment, name, desc_string, arr, nullptr, custom_data);

		_struct_size = calc_size_after_alignment(_struct_size, final_type_aligment) + type_size;
		_max_member_alignment = _max_member_alignment > final_type_aligment ? _max_member_alignment : final_type_aligment;

	} else {
		MString msg = MString::format("DataPacketDataStructureDescriptor::add_normal_member: normal member's type can not be Invaild or Structure.");
		error(msg);
		throw Exception{ msg, 0 };
	}

	return *this;
}

DataPacketDataStructureDescriptor& DataPacketDataStructureDescriptor::add_structure_member(
	DataPacketDataStructureDescriptor* struct_desc_ptr, std::string_view name, std::string_view desc_string,
	const MVector<uint32_t>& arr, uint16_t alignment, uint32_t custom_data)
{
	const observer_ptr ptr_test{ struct_desc_ptr };
	if (!ptr_test.is_object_vaild()) {
		MString msg = MString::format("DataPacketDataStructureDescriptor::add_structure_member: {} is a invaild ptr, struct is not implemented.", static_cast<void*>(struct_desc_ptr));
		error(msg);
		throw Exception{ msg, 0 };
	}

	uint16_t type_alignment = struct_desc_ptr->_struct_alignment > alignment ? struct_desc_ptr->_struct_alignment : alignment;

	uint64_t type_size = struct_desc_ptr->_struct_size;

	for (auto i : arr) {
		type_size *= i;
	}

	_members.emplace_back(DataPacketMemberType::Structure, type_size, calc_size_after_alignment(_struct_size, type_alignment), type_alignment, name, desc_string, arr, struct_desc_ptr, custom_data);

	_struct_size = calc_size_after_alignment(_struct_size, type_alignment) + type_size;
	_max_member_alignment = type_alignment > _max_member_alignment ? type_alignment : _max_member_alignment;

	return *this;
}

void DataPacketDataStructureDescriptor::compile() {
	_struct_alignment = _max_member_alignment > _struct_alignment ? _max_member_alignment : _struct_alignment;
	_struct_size = calc_size_after_alignment(_struct_size, _struct_alignment);
}

void DataPacketDataStructureDescriptor::recalcuate_and_recompile() {
	_struct_size = 0;
	_struct_alignment = _init_alignment;
	_max_member_alignment = 1;
	MVector<DataPacketDataStructureElementDescriptor> new_members = std::move(_members);

	for (auto new_member : new_members) {
		if (new_member._element_type < DataPacketMemberType::Structure) {
			add_normal_member(new_member);
		} else {
			add_structure_member(new_member._struct_ptr, new_member._name, new_member._desc, new_member._arrays, new_member._alignment);
		}
	}

	compile();
}