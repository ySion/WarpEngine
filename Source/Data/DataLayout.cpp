#include "DataBufferVector.hpp"
#include "DataLayout.hpp"
#include "DataLayoutStructureDescriptor.hpp"

#include "Core/observer_ptr.hpp"

using namespace Warp::Data;

DataLayout::DataLayout(DataLayoutStructureDescriptor* desc, uint64_t id) : _id(id) {
	_target_desc = desc;
	const observer_ptr check{ desc };
	if (!check) {
		MString msg = MString::format("DataLayout::DataLayout: desc is not vaild");
		error(msg);
		throw Exception{ msg, 0 };
	}
	compile();
}

bool DataLayout::compile() {
	if (const observer_ptr<DataLayoutStructureDescriptor> check{ _target_desc }; !check.is_object_vaild()) {
		MString msg = MString::format("DataLayout::compile: desc is not vaild");
		error(msg);
		return false;
	}

	_document_count = 0;
	_ptr_count = 0;
	_name_mapped_to_idx_offset.clear();
	_text_linear_idx.clear();
	_ptr_linear_idx.clear();

	_data_segment_size = _target_desc->get_struct_size();

	DataBufferVector name_text_segment{ 64 };
	DataBufferVector desc_text_segment{ 64 };
	DataBufferVector array_info_segment{ 64 };
	MVector<DataLayoutStructureMemberRecord> var_info{};
	var_info.reserve(64);
	uint16_t main_struct_element_count = 0;
	try {
		main_struct_element_count = visit_data_packet_desc(_target_desc, 0, 0,
								   name_text_segment, desc_text_segment, array_info_segment, var_info);
	} catch (Exception) {
		return false;
	}

	const uint32_t base_offset = sizeof(DataLayoutDataHead) + static_cast<uint32_t>(var_info.size() * sizeof(DataLayoutStructureMemberRecord));
	const uint32_t offset_name = base_offset;
	const uint32_t offset_desc = offset_name + static_cast<uint32_t>(name_text_segment.size());
	const uint32_t offset_array_info = offset_desc + static_cast<uint32_t>(desc_text_segment.size());

	for (auto& i : var_info) {
		i.name_offset += i.name_offset == 0 ? 0 : offset_name - 1;
		i.desc_offset += i.desc_offset == 0 ? 0 : offset_desc - 1;
		i.array_info_offset += i.array_info_offset == 0 ? 0 : offset_array_info - 1;
	}

	const uint32_t desc_segment_size = sizeof(DataLayoutDataHead) + var_info.size() * sizeof(DataLayoutStructureMemberRecord)
		+ name_text_segment.size() + desc_text_segment.size() + array_info_segment.size();

	_desc_segment = std::make_unique<DataBufferVector>();
	_desc_segment->reserve(desc_segment_size);

	const DataLayoutDataHead head_segment = {
		._data_element_count = var_info.size_u16(),
		._data_memeber_count_in_main_struct = main_struct_element_count,
		._memory_offset = desc_segment_size
	};

	_desc_segment->push_back(head_segment);

	_desc_segment->push_back(var_info.data(), var_info.size() * sizeof(DataLayoutStructureMemberRecord));

	_desc_segment->push_back(name_text_segment.data(), name_text_segment.size());
	_desc_segment->push_back(desc_text_segment.data(), desc_text_segment.size());
	_desc_segment->push_back(array_info_segment.data(), array_info_segment.size());

	const uint16_t main_struct_member_count = get_main_struct_memeber_count();
	_name_mapped_to_idx_offset.reserve(get_all_element_count());
	generate_name_mapped(0, main_struct_member_count, "");
	generate_ptr_data_mapped();

	return true;
}

bool DataLayout::is_idx_vaild(uint16_t idx) const {
	if (!_desc_segment || _desc_segment->data() == nullptr) return false;
	const auto count = static_cast<DataLayoutDataHead*>(_desc_segment->data())->_data_element_count;
	if (count <= idx) { return false; }
	return true;
}

uint16_t DataLayout::get_all_element_count() const {
	if (!_desc_segment || _desc_segment->data() == nullptr) return {};
	return static_cast<DataLayoutDataHead*>(_desc_segment->data())->_data_element_count;
}

uint16_t DataLayout::get_main_struct_memeber_count() const {
	if (!_desc_segment || _desc_segment->data() == nullptr) return {};
	return static_cast<DataLayoutDataHead*>(_desc_segment->data())->_data_memeber_count_in_main_struct;
}

std::optional<DataLayoutStructureMemberRecord*> DataLayout::get_member_record(uint16_t idx) const {
	if(!is_idx_vaild(idx)) { return std::nullopt; }
	const auto ptr = (DataLayoutStructureMemberRecord*)((uint64_t)_desc_segment->data() + sizeof(DataLayoutDataHead));
	return ptr + idx;
}

DataPacketMemberType DataLayout::get_memeber_type(uint16_t idx) const {
	if(const auto res = get_member_record(idx); res.has_value()) {
		return res.value()->type;
	}
	return DataPacketMemberType::Invaild;
}

std::optional<uint8_t> DataLayout::get_memeber_array_dimension(uint16_t idx) const {
	if (const auto res = get_member_record(idx); res.has_value()) {
		return res.value()->array_dimension;
	}
	return std::nullopt;
}

std::optional<uint16_t> DataLayout::get_memeber_alignment(uint16_t idx) const {
	if (const auto res = get_member_record(idx); res.has_value()) {
		if (res.value()->alignment == 0) return 256;
		return res.value()->alignment;
	}
	return std::nullopt;
}

std::optional<uint16_t> DataLayout::get_member_struct_start_idx(uint16_t idx) const {
	if (const auto res = get_member_record(idx); res.has_value()) {
		return res.value()->data_structure_offset;
	}
	return std::nullopt;
}

bool DataLayout::is_member_struct(uint16_t idx) const {
	return get_member_struct_start_idx(idx).has_value();
}

bool DataLayout::is_member_array(uint16_t idx) const
{
	return !get_member_array_info(idx).empty();
}

std::optional<uint8_t> DataLayout::get_member_struct_memeber_count(uint16_t idx) const {
	if (const auto res = get_member_record(idx); res.has_value()) {
		return res.value()->data_structure_member_count;
	}
	return std::nullopt;
}

std::string_view DataLayout::get_member_name(uint16_t idx) const {
	if (const auto res = get_member_record(idx); res.has_value()) {
		decltype(res.value()->name_offset) ptr_offest = res.value()->name_offset;
		if (ptr_offest == 0) return {};
		return std::string_view{ _desc_segment->at<const char>(ptr_offest) };
	}
	return {};
}

std::string_view DataLayout::get_member_desc_text(uint16_t idx) const {
	if (const auto res = get_member_record(idx); res.has_value()) {
		decltype(res.value()->desc_offset) ptr_offest = res.value()->desc_offset;
		if (ptr_offest == 0) return {};
		return std::string_view{ _desc_segment->at<const char>(ptr_offest) };
	}
	return {};
}

MVector<uint32_t> DataLayout::get_member_array_info(uint16_t idx) const {
	auto res = get_memeber_array_dimension(idx);
	if(!res.has_value()) return {};

	const uint16_t array_dimension = res.value();

	if (const auto res = get_member_record(idx); res.has_value()) {
		decltype(res.value()->array_info_offset) ptr_offest = res.value()->array_info_offset;
		if (ptr_offest == 0) return {};
		MVector<uint32_t> result{ array_dimension };
		const uint32_t* array_ptr = _desc_segment->at<uint32_t>(ptr_offest);
		memcpy(result.data(), array_ptr, array_dimension * sizeof(uint32_t));
		return result;
	}
	return {};
}

std::optional<uint64_t> DataLayout::get_member_size(uint16_t idx) const {
	if (const auto res = get_member_record(idx); res.has_value()) {
		return res.value()->data_size;
	}
	return std::nullopt;
}

std::optional<uint32_t> DataLayout::get_member_custom_data(uint16_t idx) const {
	if (const auto res = get_member_record(idx); res.has_value()) {
		return res.value()->custom_data;
	}
	return std::nullopt;
}

std::optional<uint64_t> DataLayout::get_member_data_start_address_offset(uint16_t idx) const {
	if (const auto res = get_member_record(idx); res.has_value()) {
		return res.value()->data_offset;
	}
	return std::nullopt;
}

std::optional<uint64_t> DataLayout::get_member_data_start_address_offset_by_name(const MString& str) {
	if (const auto ptr = _name_mapped_to_idx_offset.find(str); ptr != _name_mapped_to_idx_offset.end()) {
		return ptr->second.address_offset;
	}
	return std::nullopt;
}

std::optional<uint16_t> DataLayout::get_member_data_idx_by_name(const MString& str) const {
	if (const auto ptr = _name_mapped_to_idx_offset.find(str); ptr != _name_mapped_to_idx_offset.end()) {
		return ptr->second.idx_offset;
	}
	return std::nullopt;
}

std::optional<uint16_t> DataLayout::get_member_linear_idx_document(uint16_t idx) const {
	if (const auto ptr_linear = _text_linear_idx.find(idx); ptr_linear != _text_linear_idx.end()) {
		return ptr_linear->second;
	}
	return std::nullopt;
}

std::optional<uint16_t> DataLayout::get_member_linear_idx_ptr(uint16_t idx) const {
	if (const auto ptr_linear = _ptr_linear_idx.find(idx); ptr_linear != _ptr_linear_idx.end()) {
		return ptr_linear->second;
	}
	return std::nullopt;
}

std::optional<uint16_t> DataLayout::get_member_linear_idx_document_by_name(const MString& str) const {
	if (const auto v = get_member_data_idx_by_name(str); v.has_value()) {
		return get_member_linear_idx_document(v.value());
	}
	return  std::nullopt;
}

std::optional<uint16_t> DataLayout::get_member_linear_idx_ptr_by_name(const MString& str) const {
	if (const auto v = get_member_data_idx_by_name(str); v.has_value()) {
		return get_member_linear_idx_ptr(v.value());
	}
	return  std::nullopt;
}

MVector<uint64_t> DataLayout::get_all_member_address_offset_by_type(DataPacketMemberType type) const {
	MVector<uint64_t> result{};
	result.reserve(_ptr_count);
	const uint16_t count = get_all_element_count();
	for (uint16_t i = 0; i < count; ++i) {
		if (get_memeber_type(i) == type) {
			result.push_back(get_member_data_start_address_offset(i).value());
		}
	}
	return result;
}

MVector<uint16_t> DataLayout::get_all_member_idx_by_type(DataPacketMemberType type) const {
	MVector<uint16_t> result{};
	result.reserve(_ptr_count);
	const uint16_t count = get_all_element_count();
	for (uint16_t i = 0; i < count; ++i) {
		if (get_memeber_type(i) == type) {
			result.push_back(i);
		}
	}
	return result;
}

uint32_t DataLayout::get_info_segment_size() const {
	return _desc_segment->size();
}

uint16_t DataLayout::visit_data_packet_desc(DataLayoutStructureDescriptor* head, uint8_t depth,
						  uint64_t base_offset, DataBufferVector& name_text_segment, DataBufferVector& desc_text_segment,
						  DataBufferVector& array_info_segment, MVector<DataLayoutStructureMemberRecord>& var_info) {

	const observer_ptr check{ head };
	if (!check.is_object_vaild()) {
		MString msg = MString::format("DataLayout::visit_data_packet_desc: desc is not vaild");
		error(msg);
		throw Exception{ msg, 0 };
	}
	uint16_t memeber_count = 0;

	const auto& members_ref = head->get_members();

	//结构体的record index, 对应描述, 数据地址偏移
	MVector<std::tuple<uint32_t, const DataPacketDataStructureElementDescriptor*, uint64_t>> struct_member{};

	for (const auto& i : members_ref) {
		DataLayoutStructureMemberRecord record{};
		record.type = i._element_type;
		record.array_dimension = i._arrays.size_u8();
		record.alignment = (uint8_t)(i._alignment);
		record.data_structure_offset = 0; // later set if need
		record.data_structure_member_count = 0;
		record.custom_data = i.custom_data;

		record.data_offset = base_offset + i._element_offset;
		record.data_size = i._element_size;

		if (record.type == DataPacketMemberType::Document) {
			_document_count++;
		} else if (record.type == DataPacketMemberType::Ptr) {
			_ptr_count++;
		} else if (record.type == DataPacketMemberType::Structure) {
			record.data_structure_member_count = (uint8_t)(i._struct_ptr->get_current_member_count());
		}

		using type_name_offset = decltype(record.name_offset);
		using type_desc_offset = decltype(record.desc_offset);
		using type_array_info_offset = decltype(record.array_info_offset);

		if (i._name.empty()) {
			record.name_offset = 0;
		} else {
			record.name_offset = static_cast<type_name_offset>(name_text_segment.size()) + 1;
			name_text_segment.push_back(i._name.c_str(), i._name.size() + 1);
		}

		if (i._desc.empty()) {
			record.desc_offset = 0;
		} else {
			record.desc_offset = static_cast<type_desc_offset>(desc_text_segment.size()) + 1;
			desc_text_segment.push_back(i._desc.c_str(), i._desc.size() + 1);
		}

		if (i._arrays.empty()) {
			record.array_info_offset = 0;
		} else {
			record.array_info_offset = static_cast<type_array_info_offset>(array_info_segment.size()) + 1;
			for (int j = 0; j < record.array_dimension; ++j) {
				array_info_segment.push_back(i._arrays[j]);
			}
		}

		var_info.emplace_back(record);

		if (record.type == DataPacketMemberType::Structure) {
			warn("add strcut {}", depth, i._name);
			struct_member.push_back({ var_info.size_u32() - 1, &i, record.data_offset });
		}
		memeber_count++;
	}

	for (const auto& i : struct_member) {
		warn("{} to {}", depth, std::get<1>(i)->_name);
		var_info[std::get<0>(i)].data_structure_offset = var_info.size_u16();
		visit_data_packet_desc(std::get<1>(i)->_struct_ptr, depth + 1, std::get<2>(i), name_text_segment, desc_text_segment, array_info_segment, var_info);
	}

	return memeber_count;
}

void DataLayout::generate_name_mapped(uint16_t offest, uint16_t member_count, const MString& base_string) {
	for (uint16_t i = offest; i < offest + member_count; ++i) {
		MString name = base_string;
		name += get_member_name(i);
		info("{}", name);
		_name_mapped_to_idx_offset[name] = DataPacketMemberOffsetInfo{ get_member_data_start_address_offset(i).value(), i, };

		if(is_member_array(i)) {
			continue;
		}

		if (is_member_struct(i)) {
			const auto idx = get_member_struct_start_idx(i).value();
			generate_name_mapped(get_member_struct_start_idx(i).value(), get_member_struct_memeber_count(i).value(), name + ".");
		}
	}
}

void DataLayout::generate_ptr_data_mapped() {
	const auto text_offsets = get_all_member_idx_by_type(DataPacketMemberType::Document);
	const auto ptr_offsets = get_all_member_idx_by_type(DataPacketMemberType::Ptr);

	_text_linear_idx.reserve(text_offsets.size());
	_ptr_linear_idx.reserve(ptr_offsets.size());

	for (uint16_t i = 0; i < text_offsets.size_u16(); ++i) {
		_text_linear_idx[text_offsets[i]] = i;
	}

	for (uint16_t i = 0; i < ptr_offsets.size_u16(); ++i) {
		_ptr_linear_idx[ptr_offsets[i]] = i;
	}
}
