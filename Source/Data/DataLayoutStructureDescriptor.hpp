#pragma once

#include "Core/Core.hpp"
#include "DataTypes.hpp"

WARP_TYPE_NAME_2(Data, DataLayoutStructureDescriptor);

namespace Warp::Data {

	inline uint64_t calc_size_after_alignment(uint64_t size, uint16_t alignment) {
		return size + (alignment - size % alignment) % alignment;
	}

	struct DataPacketDataStructureElementDescriptor {
		DataPacketMemberType _element_type{};
		uint64_t _element_size{ 0 };
		uint64_t _element_offset{ 0 };
		uint16_t _alignment{ 1 };
		MString _name{};
		MString _desc{};
		MVector<uint32_t> _arrays{}; // for example {32, 16, 8 } equal  var[32][16][8]
		DataLayoutStructureDescriptor* _struct_ptr{}; // 如果是struct, 那么struct名字是什么
		uint32_t custom_data;
	};

	//对齐规则: 基础类型, 对齐永远基础类型自己身的大小
	//结构体嵌套对齐:  结构体中最大的基础类型的对齐 或者 自己指定的结构体对齐大小(但是这个指定大小必须大于等于结构体中最大的基础类型的对齐)
	//结构体大小: 永远是结构体中最大的对齐的整数倍
	class DataLayoutStructureDescriptor : public Inherit<DataLayoutStructureDescriptor, Object> {
	public:
		DataLayoutStructureDescriptor() = delete;

		DataLayoutStructureDescriptor(std::string_view struct_name = "",
			std::string_view desc_text = "", uint16_t alignment = 1);

		~DataLayoutStructureDescriptor() override = default;

		//对齐规则: 对齐1 ~ 256
		DataLayoutStructureDescriptor& add_normal_member(DataPacketMemberType type,
			std::string_view name = "",
			std::string_view desc_string = "",
			const MVector<uint32_t>& arr = {},
			uint16_t alignment = 1, 
			uint32_t custom_data = 0);

		inline DataLayoutStructureDescriptor& add_normal_member(const DataPacketDataStructureElementDescriptor& element_desc) {
			return add_normal_member(element_desc._element_type, element_desc._name, element_desc._desc, element_desc._arrays, element_desc._alignment, element_desc.custom_data);
		}

		DataLayoutStructureDescriptor& add_structure_member(DataLayoutStructureDescriptor* struct_desc_ptr,
			std::string_view name = "",
			std::string_view desc_string = "",
			const MVector<uint32_t>& arr = {},
			uint16_t alignment = 1, 
			uint32_t custom_data = 0);

		void compile();

		void recalcuate_and_recompile();

		inline uint8_t get_current_member_count() const {
			return _members.size_u8();
		}

		inline uint32_t get_all_member_count() const {

			uint32_t size = 0;
			for(auto& member : _members) {
				size++;
				if(member._element_type == DataPacketMemberType::Structure) {
					size += member._struct_ptr->get_all_member_count();
				}
			}
			return size;
		}


		inline uint64_t get_struct_size() const { return _struct_size; }

		inline uint16_t get_struct_alignment() const { return _struct_alignment; }

		inline uint16_t get_max_member_alignment() const { return _max_member_alignment; }

		inline const MString& get_struct_name() const { return _name; }

		inline const MString& get_struct_desc_text() const { return _desc_text; }

		inline const MVector<DataPacketDataStructureElementDescriptor>& get_members() const { return _members; }

		inline void for_each_read(auto func, uint8_t depth = 0) {

			for (const auto& member : _members) {
				func(member, depth);
				if(member._element_type == DataPacketMemberType::Structure) {
					member._struct_ptr->for_each_read(func, depth + 1);
				}
			}
		}

	private:
		uint64_t _struct_size{ 0 };
		uint16_t _init_alignment{ 1 };
		uint16_t _struct_alignment{ 1 };
		uint16_t _max_member_alignment{ 1 };
		MString _name{};
		MString _desc_text{};
		MVector<DataPacketDataStructureElementDescriptor> _members{};
		MSet<MString> _member_name_set{};
	};
}