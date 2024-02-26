#pragma once

#include "Core/Core.hpp"

WARP_TYPE_NAME_2(Data, DataLayout);

namespace Warp::Data {

	class DataBufferVector;

	class DataLayoutStructureDescriptor;

	struct DataLayoutDataHead {
		uint16_t _data_element_count{};
		uint16_t _data_memeber_count_in_main_struct{};
		uint32_t _memory_offset{};
		uint64_t _hash;
	};

	struct DataLayoutStructureMemberRecord {
		DataPacketMemberType type;		// 类型
		uint8_t array_dimension;		// 数组维度

		uint8_t alignment;			// 对齐  0的时候是256对齐, 默认是1

		uint8_t data_structure_member_count;	// 如果是结构体, 那么结构体有几个成员
		uint16_t data_structure_offset;		// 如果是结构体, 那么结构体信息的起始地址
		uint16_t array_info_offset;		// 数组信息 起始地址

		uint16_t name_offset;			// 名字偏移
		uint16_t desc_offset;			// 描述字符串 起始地址

		uint32_t custom_data;			// 自定义数据
		uint64_t data_offset;			// 数据起始地址 (注意, 就算是指针, 也是一个u64_t的值!
		uint64_t data_size;			// 数据大小
	};

	/// layout memory:
	/// info segment
	/// |                                 head segment                                 |                         desc segment                        | array_info | name_text |  desc_text |
	/// | var_count(16bit) + main_struct_var_count(16bit) + memory_start_offset(32bit) + hash(64_byte) | (var_count * sizeof(DataLayoutStructureMemberRecord)) | array_info | name_text |  desc_text |
	class DataLayout : public Inherit<DataLayout, Object> {
		struct DataPacketMemberOffsetInfo {
			uint64_t address_offset;
			uint16_t idx_offset;
			
		};

		friend class DataLayoutManager;
	public:
		DataLayout(DataLayoutStructureDescriptor* desc, uint64_t id);

		DataLayout() = delete;

		bool compile();

		bool is_idx_vaild(uint16_t idx) const;

		uint16_t get_all_element_count() const;

		uint16_t get_main_struct_memeber_count() const;

		std::optional<DataLayoutStructureMemberRecord*> get_member_record(uint16_t idx) const;

		DataPacketMemberType get_memeber_type(uint16_t idx) const;

		std::optional<uint8_t> get_memeber_array_dimension(uint16_t idx) const;

		std::optional<uint16_t>  get_memeber_alignment(uint16_t idx) const;

		std::optional<uint16_t> get_member_struct_start_idx(uint16_t idx) const;

		bool is_member_struct(uint16_t idx) const;

		bool is_member_array(uint16_t idx) const;

		std::optional<uint8_t> get_member_struct_memeber_count(uint16_t idx) const;

		std::string_view get_member_name(uint16_t idx) const;

		std::string_view get_member_desc_text(uint16_t idx) const;

		MVector<uint32_t> get_member_array_info(uint16_t idx) const;

		std::optional<uint64_t> get_member_size(uint16_t idx) const;

		std::optional<uint32_t> get_member_custom_data(uint16_t idx) const;

		std::optional<uint64_t> get_member_data_start_address_offset(uint16_t idx) const;

		std::optional<uint64_t> get_member_data_start_address_offset_by_name(const MString& str);

		std::optional<uint16_t> get_member_data_idx_by_name(const MString& str) const;

		std::optional<uint16_t> get_member_linear_idx_document(uint16_t idx) const;

		std::optional<uint16_t> get_member_linear_idx_ptr(uint16_t idx) const;

		std::optional<uint16_t> get_member_linear_idx_document_by_name(const MString& str) const;

		std::optional<uint16_t> get_member_linear_idx_ptr_by_name(const MString& str) const;

		MVector<uint64_t> get_all_member_address_offset_by_type(DataPacketMemberType type) const;

		MVector<uint16_t> get_all_member_idx_by_type(DataPacketMemberType type) const;

		uint32_t get_info_segment_size() const;

		inline uint32_t get_memeber_document_count() const { return _document_count; }

		inline uint32_t get_memeber_ptr_count() const { return _ptr_count; }

		inline bool is_pure() const { return _document_count == 0 && _ptr_count == 0; }

		inline uint64_t get_data_segment_size() const { return _data_segment_size; }

		inline MMap<MString, DataPacketMemberOffsetInfo>& get_name_mapped_to_idx_offset() { return _name_mapped_to_idx_offset; }

		inline void visit(std::function<void(uint16_t, uint16_t)> func) {
			for (uint16_t i = 0; i < get_main_struct_memeber_count(); i++) {
				func(i, 0);
				if (is_member_struct(i)) {
					const auto sub_count = get_member_struct_memeber_count(i).value();
					visit_sub(func, get_member_struct_start_idx(i).value(), sub_count, 1);
				}
			}
		}

		inline uint64_t get_id() const { return _id; }
	private:
		uint16_t visit_data_packet_desc(DataLayoutStructureDescriptor* head,
			uint8_t depth,
			uint64_t base_offset,
			DataBufferVector& name_text_segment,
			DataBufferVector& desc_text_segment,
			DataBufferVector& array_info_segment,
			MVector<DataLayoutStructureMemberRecord>& var_info);

		void generate_name_mapped(uint16_t offest, uint16_t member_count, const MString& base_string);

		void generate_ptr_data_mapped();

		inline void visit_sub(std::function<void(uint16_t, uint16_t)>& func, uint16_t idx, uint16_t count, uint16_t depth) {
			for (uint16_t i = idx; i < idx + count; i++) {
				func(i, depth);
				if (is_member_struct(i)) {
					const auto sub_count = get_member_struct_memeber_count(i).value();
					visit_sub(func, get_member_struct_start_idx(i).value(), sub_count, depth + 1);
				}
			}
		}

	private:
		uint64_t _id{};
		DataLayoutStructureDescriptor* _target_desc{};
		std::unique_ptr<DataBufferVector> _desc_segment{};
		uint64_t _data_segment_size{};
		uint32_t _document_count{};
		uint32_t _ptr_count{};
		MMap<MString, DataPacketMemberOffsetInfo> _name_mapped_to_idx_offset{};
		MMap<uint16_t, uint16_t> _text_linear_idx{};
		MMap<uint16_t, uint16_t> _ptr_linear_idx{};
	};
}