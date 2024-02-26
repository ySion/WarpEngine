#include "DataLayoutStructureDescriptor.hpp"

#include "Core/observer_ptr.hpp"
#include "Core/Exception.hpp"
#include "Core/Logger.hpp"

using namespace Warp::Data;

DataLayoutStructureDescriptor::DataLayoutStructureDescriptor(std::string_view struct_name,
                                                                     std::string_view desc_text, uint16_t alignment) {
	_name = struct_name;
	_desc_text = desc_text;
	_struct_alignment = alignment;
	_init_alignment = alignment;
}

DataLayoutStructureDescriptor& DataLayoutStructureDescriptor::add_normal_member(DataPacketMemberType type,
	std::string_view name, std::string_view desc_string, const MVector<uint32_t>& arr, uint16_t alignment, uint32_t custom_data)
{
	if(_member_name_set.contains(name)){
		MString msg = MString::format("DataLayoutStructureDescriptor::add_normal_member: member name '{}' is already used, add member failed, it wiil be ignored.", name);
		warn(msg);
		return *this;
	}

	if(_members.size() >= 255) {
		MString msg = MString::format("DataLayoutStructureDescriptor::add_normal_member: member count is limit at 255, you can't apply any more, add member '{}' failed, it wiil be ignored.", name);
		warn(msg);
		return *this;
	}

	if(alignment > 256 && alignment == 0) {
		MString msg = MString::format("DataLayoutStructureDescriptor::add_normal_member: alignment must be in range [1, 256], fallback to alignment = 1.");
		warn(msg);
		alignment = 1;
	}

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

		_member_name_set.insert(name);
	} else {
		MString msg = MString::format("DataLayoutStructureDescriptor::add_normal_member: normal member's type can not be Invaild or Structure.");
		error(msg);
		throw Exception{ msg, 0 };
	}

	return *this;
}

DataLayoutStructureDescriptor& DataLayoutStructureDescriptor::add_structure_member(
	DataLayoutStructureDescriptor* struct_desc_ptr, std::string_view name, std::string_view desc_string,
	const MVector<uint32_t>& arr, uint16_t alignment, uint32_t custom_data)
{
	const observer_ptr ptr_test{ struct_desc_ptr };
	if (!ptr_test.is_object_vaild()) {
		MString msg = MString::format("DataLayoutStructureDescriptor::add_structure_member: {} is a invaild ptr, struct is not implemented.", static_cast<void*>(struct_desc_ptr));
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

void DataLayoutStructureDescriptor::compile() {
	_struct_alignment = _max_member_alignment > _struct_alignment ? _max_member_alignment : _struct_alignment;
	_struct_size = calc_size_after_alignment(_struct_size, _struct_alignment);
}

void DataLayoutStructureDescriptor::recalcuate_and_recompile() {
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