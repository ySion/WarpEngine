#pragma once
#include "DataPacketDataStructreInfoPack.hpp"

#include "Core/Inherit.hpp"
#include "Core/Exception.hpp"
#include "Core/Logger.hpp"
#include "Core/observer_ptr.hpp"

WARP_TYPE_NAME_2(Data, DataBuffer);
WARP_TYPE_NAME_2(Data, DataBufferVector);
WARP_TYPE_NAME_2(Data, DataBufferUniformVector);
WARP_TYPE_NAME_2(Data, DataPacket);
WARP_TYPE_NAME_2(Data, DataPacketVector);

namespace Warp::Data {

	class DataBuffer : public Inherit<DataBuffer, Object> {
	public:

		/// 不分配内存, 后续可以通过resize来分配
		/// 默认4字节对齐
		DataBuffer(int aligment = 4) :_size(0), _aligment(aligment) {}


		DataBuffer(uint64_t size, int aligment = 4) : _size(size), _aligment(aligment) {

			_data = static_cast<uint8_t*>(mi_malloc_aligned(size, _aligment));

			if (_data == nullptr) {
				MString msg = MString::format("DataBuffer::DataBuffer: failed to allocate memory, size: {}, aligment: {}", _size, _aligment);
				error(msg);
				throw Exception{ msg, 0 };
			}
		}

		~DataBuffer() override {
			if (_data != nullptr) {
				mi_free_aligned(_data, _aligment);
			}
		}

		void resize(uint64_t new_size) {

			if (_data == nullptr) {
				_data = static_cast<uint8_t*>(mi_malloc_aligned(new_size, _aligment));
			} else {
				_data = static_cast<uint8_t*>(mi_realloc_aligned(_data, new_size, _aligment));
			}

			if (_data == nullptr) {
				MString msg = MString::format("DataBuffer::DataBuffer: failed to resize memory, new size: {}, new aligment: {}", _size, _aligment);
				error(msg);
				throw Exception{ msg, 0 };
			}

			_size = new_size;
		}

		inline bool is_vailed() const { return _data != nullptr; }

		inline uint64_t get_size() const { return _size; }

		inline uint8_t* get_buffer() const { return _data; }
	private:
		uint64_t _size{ 0 };
		uint8_t* _data{};
		const uint16_t _aligment{ 4 };
	};

	class DataBufferVector : public Inherit<DataBufferVector, Object> {
	public:
		DataBufferVector() {
			_buffer.resize(64);
		}

		DataBufferVector(uint64_t init_memory_size) {
			_buffer.resize(init_memory_size);
		}

		~DataBufferVector() override = default;

		template<class T> bool push_back(const T& element) {
			return push_back(&element, sizeof(T));
		}

		bool push_back(const void* p, uint64_t size) {
			if (p == nullptr) return false;
			if (size == 0) return true;

			try {
				if (get_all_memory_size() < _offset + size) {

					if (static_cast<uint64_t>(1.5 * get_all_memory_size()) >= _offset + size) {
						_buffer.resize(static_cast<uint64_t>(1.5 * get_all_memory_size()));
					} else {
						_buffer.resize(get_all_memory_size() + _offset + size);
					}
				}

			} catch (Exception e) {
				return false;
			}

			memcpy(_buffer.get_buffer() + _offset, p, size);

			_offset += size;
			return true;
		}

		template<class T> bool fetch_back(T& element) {
			return fetch_back(&element, sizeof(T));
		}

		bool fetch_back(void* p, uint64_t size) {
			if (p == nullptr) return false;

			if (_offset < size) { return false; }

			_offset -= size;
			memcpy_s(p, size, _buffer.get_buffer() + _offset, size);

			return true;
		}


		template<class T> bool read_at(uint64_t address_offset, T* p) {
			if (p == nullptr) return false;

			if (address_offset + sizeof(T) > _offset) { return false; }

			memcpy(p, _buffer.get_buffer() + address_offset, sizeof(T));

			return true;
		}

		bool read_at(uint64_t address_offset, void* p, uint64_t size) const {
			if (p == nullptr) return false;

			if (address_offset + size > _offset) { return false; }

			memcpy(p, _buffer.get_buffer() + address_offset, size);

			return true;
		}

		template<class T> T* at(uint64_t address_offset) {
			return static_cast<T*>(at(address_offset));
		}

		void* at(uint64_t address_offset) const {

			if (address_offset >= get_used_memory_size()) {
				return nullptr;
			}
			return _buffer.get_buffer() + address_offset;
		}

		void clear() {
			_offset = 0;
		}

		void reserve(uint64_t size) {
			if (get_all_memory_size() < size) {
				_buffer.resize(size);
			}
		}

		inline void* get_data() const { return _buffer.get_buffer(); }

		inline uint64_t get_used_memory_size() const { return _offset; }

		inline uint64_t get_all_memory_size() const { return _buffer.get_size(); };

	public:
		DataBuffer _buffer{};
		uint64_t _offset{ 0 };
	};

	class DataBufferUniformVector : public Inherit<DataBufferUniformVector, Object> {
	public:
		DataBufferUniformVector(uint64_t element_size_byte) {
			_element_size = element_size_byte;
		}

		~DataBufferUniformVector() override = default;

		template<class T> bool push_back(const T& element) {
			if (sizeof(T) != _element_size) {
				MString msg = MString::format("DataBufferUniformVector::push_back: element size not match, element size: {}, uniform element size: {}", sizeof(T), _element_size);
				error(msg);
				return false;
			}

			return _vector.push_back(&element, _element_size);
		}

		inline bool push_back(const void* p) {
			return _vector.push_back(p, _element_size);
		}


		template<class T> bool fetch_back(T& element) {
			if (sizeof(T) != _element_size) {
				MString msg = MString::format("DataBufferUniformVector::push_back: element size not match, element size: {}, uniform element size: {}", sizeof(T), _element_size);
				error(msg);
				return false;
			}
			return fetch_back(&element, sizeof(T));
		}

		inline bool fetch_back(void* p) {
			return _vector.fetch_back(p, _element_size);
		}

		inline void reserve(uint64_t element_count) {
			_vector.reserve(_element_size * element_count);
		}

		template<class T> T* at(uint64_t index) const {
			return static_cast<T*>(at(index));
		}

		inline void* at(uint64_t index) const {
			const uint64_t offset = index * _element_size;
			if (index >= get_count()) {
				return nullptr;
			}
			return static_cast<uint8_t*>(_vector.get_data()) + offset;
		}

		inline uint64_t get_count() const {
			return _vector.get_used_memory_size() / _element_size;
		}

		inline void* get_data() const { return _vector.get_data(); }

		inline uint64_t get_used_memory_size() const { return _vector.get_used_memory_size(); }

		inline uint64_t get_all_memory_size() const { return _vector.get_all_memory_size(); };
	public:
		uint64_t _element_size;
		DataBufferVector _vector{};
	};

	struct DataPacketLayoutDataHead {
		uint16_t _data_element_count{};
		uint16_t _data_memeber_count_in_main_struct{};
		uint32_t _memory_offset{};
	};

	struct DataPacketLayoutStructureMemberRecord {
		DataPacketMemberType type;		// 类型
		uint8_t array_dimension;		// 数组维度

		uint16_t alignment;			// 对齐

		uint16_t data_structure_offset;		// 如果是结构体, 那么结构体信息的起始地址
		uint16_t data_structure_member_count;	// 如果是结构体, 那么结构体有几个成员

		uint32_t name_offset;			// 名字偏移
		uint32_t desc_offset;			// 描述字符串 起始地址

		uint32_t array_info_offset;		// 数组信息 起始地址
		uint32_t custom_data;			// 自定义数据
		uint64_t data_offset;			// 数据起始地址 (注意, 就算是指针, 也是一个u64_t的值!
		uint64_t data_size;			// 数据大小
	};

	/// layout memory:
	/// |                                 head segment                                 |                         desc segment                        | array_info | name_text |  desc_text |
	/// | var_count(16bit) + main_struct_var_count(16bit) + memory_start_offest(32bit) | (var_count * sizeof(DataPacketLayoutStructureMemberRecord)) | array_info | name_text |  desc_text |
	class DataPacketLayout : public Inherit<DataPacketLayout, Object> {
	public:
		DataPacketLayout() = delete;

		DataPacketLayout(DataPacketDataStructureDescriptor* desc) {
			_target_desc = desc;
			if (!_target_desc) {
				MString msg = MString::format("DataPacketLayout::DataPacketLayout: desc is not vaild");
				error(msg);
				throw Exception{ msg, 0 };
			}
			_data_segment_size = desc->get_struct_size();
		}

		bool compile() {
			if (!_target_desc.is_object_vaild()) {
				MString msg = MString::format("DataPacketLayout::compile: desc is not vaild");
				error(msg);
				return false;
			}

			DataBufferVector name_text_segment{};
			DataBufferVector desc_text_segment{};
			DataBufferVector array_info_segment{};
			MVector<DataPacketLayoutStructureMemberRecord> var_info{};

			uint16_t main_struct_element_count = 0;
			try {
				main_struct_element_count = visit_data_packet_desc(_target_desc.get(), 0, 0, 
					name_text_segment, desc_text_segment, array_info_segment, var_info);
			} catch (Exception) {
				return false;
			}

			const uint32_t base_offset = sizeof(DataPacketLayoutDataHead) + static_cast<uint32_t>(var_info.size() * sizeof(DataPacketLayoutStructureMemberRecord));
			const uint32_t offset_name = base_offset;
			const uint32_t offset_desc = offset_name + static_cast<uint32_t>(name_text_segment.get_used_memory_size());
			const uint32_t offset_array_info = offset_desc + static_cast<uint32_t>(desc_text_segment.get_used_memory_size());

			for (auto& i : var_info) {
				i.name_offset += i.name_offset == std::numeric_limits<uint32_t>::max() ? 0 : offset_name;
				i.desc_offset += i.desc_offset == std::numeric_limits<uint32_t>::max() ? 0 :  offset_desc;
				i.array_info_offset += i.array_info_offset == std::numeric_limits<uint32_t>::max() ? 0 :  offset_array_info;
			}

			const uint32_t desc_segment_size = sizeof(DataPacketLayoutDataHead) + var_info.size() * sizeof(DataPacketLayoutStructureMemberRecord)
				+ name_text_segment.get_used_memory_size() + desc_text_segment.get_used_memory_size() + array_info_segment.get_used_memory_size();

			_desc_segment = std::make_unique<DataBufferVector>();
			_desc_segment->reserve(desc_segment_size);

			const DataPacketLayoutDataHead head_segment = {
				._data_element_count = var_info.size_u16(),
				._data_memeber_count_in_main_struct = main_struct_element_count,
				._memory_offset = desc_segment_size
			};

			_desc_segment->push_back(head_segment);

			_desc_segment->push_back(var_info.data(), var_info.size() * sizeof(DataPacketLayoutStructureMemberRecord));

			info("{}, size {}.", (void*)_desc_segment->get_data(), _desc_segment->get_used_memory_size());

			_desc_segment->push_back(name_text_segment.get_data(), name_text_segment.get_used_memory_size());
			_desc_segment->push_back(desc_text_segment.get_data(), desc_text_segment.get_used_memory_size());
			_desc_segment->push_back(array_info_segment.get_data(), array_info_segment.get_used_memory_size());

			info("{}, size {}.", (void*)name_text_segment.get_data(), name_text_segment.get_used_memory_size());
			info("{}, size {}.", (void*)desc_text_segment.get_data(), desc_text_segment.get_used_memory_size());
			info("{}, size {}.", (void*)array_info_segment.get_data(), array_info_segment.get_used_memory_size());
			info("{}, size {}.", (void*)_desc_segment->get_data(), _desc_segment->get_used_memory_size());

			return true;
		}

		uint16_t visit_data_packet_desc(DataPacketDataStructureDescriptor* head, 
			uint8_t depth, 
			uint64_t base_offset, 
			DataBufferVector& name_text_segment, 
			DataBufferVector& desc_text_segment, 
			DataBufferVector& array_info_segment, 
			MVector<DataPacketLayoutStructureMemberRecord>& var_info)
		{
			const observer_ptr check{ head };    
			if (!check.is_object_vaild()) {
				MString msg = MString::format("DataPacketLayout::visit_data_packet_desc: desc is not vaild");
				error(msg);
				throw Exception{ msg, 0 };
			}
			uint16_t memeber_count = 0;

			DataPacketLayoutStructureMemberRecord record{};

			const auto& members_ref = head->get_members();

			const uint16_t current_struct_member_count = head->get_current_member_count();

			MVector<std::pair<uint32_t, const DataPacketDataStructureElementDescriptor*>> struct_member{};

			for (const auto& i : members_ref) {
				DataPacketLayoutStructureMemberRecord record{};
				record.type = i._element_type;
				record.array_dimension = i._arrays.size_u8();
				record.alignment = i._alignment;
				record.data_structure_offset = 0; // later set
				record.data_structure_member_count = current_struct_member_count;
				record.custom_data = i.custom_data;

				record.data_offset = base_offset + i._element_offset;
				record.data_size = i._element_size;

				if (i._name.empty()) {
					record.name_offset = std::numeric_limits<uint32_t>::max();
				} else {
					record.name_offset = static_cast<uint32_t>(name_text_segment.get_used_memory_size());
					name_text_segment.push_back(i._name.c_str(), i._name.size() + 1);
				}

				if (i._desc.empty()) {
					record.desc_offset = std::numeric_limits<uint32_t>::max();
				} else {
					record.desc_offset = static_cast<uint32_t>(desc_text_segment.get_used_memory_size());
					desc_text_segment.push_back(i._desc.c_str(), i._desc.size() + 1);
				}

				if (i._arrays.empty()) {
					record.array_info_offset = std::numeric_limits<uint32_t>::max();
				} else {
					record.array_info_offset = static_cast<uint32_t>(array_info_segment.get_used_memory_size());
					for (int j = 0; j < record.array_dimension; ++j) {
						array_info_segment.push_back(i._arrays[j]);
					}
				}

				var_info.emplace_back(record);

				if (record.type == DataPacketMemberType::Structure) {
					struct_member.push_back({ var_info.size_u32() - 1, &i});
				}
				memeber_count++;
			}

			for (const auto& i : struct_member) {
				var_info[i.first].data_structure_offset = var_info.size_u32();
				visit_data_packet_desc(i.second->_struct_ptr, depth + 1, record.data_offset, name_text_segment, desc_text_segment, array_info_segment, var_info);
			}
			
			return memeber_count;
		}

		uint32_t get_data_all_element_count() const {
			if(const auto ptr = _desc_segment->at<uint16_t>(0); ptr != nullptr) {
				return *ptr;
			}
			return 0;
		}

		uint32_t get_data_main_struct_memeber_count() const {
			if (const auto ptr = _desc_segment->at<uint16_t>(2); ptr != nullptr) {
				return *ptr;
			}
			return 0;
		}

		DataPacketMemberType get_data_memeber_info_type(uint16_t idx) const {
			uint32_t offset = sizeof(DataPacketLayoutDataHead) + idx * sizeof(DataPacketLayoutStructureMemberRecord);
			if (const auto ptr = _desc_segment->at<DataPacketMemberType>(offset); ptr != nullptr) {
				return *ptr;
			}
			return DataPacketMemberType::Invaild;
		}

		uint8_t get_data_memeber_info_array_dimension(uint16_t idx) const {
			uint32_t offset = sizeof(DataPacketLayoutDataHead) + idx * sizeof(DataPacketLayoutStructureMemberRecord);
			constexpr uint32_t _offset2 = offsetof(DataPacketLayoutStructureMemberRecord, array_dimension);
			if (const auto ptr = _desc_segment->at<uint8_t>(offset + _offset2); ptr != nullptr) {
				return *ptr;
			}
			return 0;
		}

		uint16_t get_data_memeber_info_array_alignment(uint16_t idx) const {
			const uint32_t offset = sizeof(DataPacketLayoutDataHead) + idx * sizeof(DataPacketLayoutStructureMemberRecord);
			constexpr uint32_t _offset2 = offsetof(DataPacketLayoutStructureMemberRecord, alignment);
			if (const auto ptr = _desc_segment->at<uint16_t>(offset + _offset2); ptr != nullptr) {
				return *ptr;
			}
			return 0;
		}

		uint16_t get_data_memeber_info_member_struct_start_idx(uint16_t idx) const {
			const uint32_t offset = sizeof(DataPacketLayoutDataHead) + idx * sizeof(DataPacketLayoutStructureMemberRecord);
			constexpr uint32_t _offset2 = offsetof(DataPacketLayoutStructureMemberRecord, data_structure_offset);
			if (const auto ptr = _desc_segment->at<uint16_t>(offset + _offset2); ptr != nullptr) {
				return *ptr;
			}
			return 0;
		}

		bool get_data_memeber_info_is_member_struct(uint16_t idx) const {
			return get_data_memeber_info_member_struct_start_idx(idx) != 0;
		}

		uint16_t get_data_memeber_info_member_struct_memeber_count(uint16_t idx) const {
			const uint32_t offset = sizeof(DataPacketLayoutDataHead) + idx * sizeof(DataPacketLayoutStructureMemberRecord);
			constexpr uint32_t _offset2 = offsetof(DataPacketLayoutStructureMemberRecord, data_structure_member_count);
			if (const auto ptr = _desc_segment->at<uint16_t>(offset + _offset2); ptr != nullptr) {
				return *ptr;
			}
			return 0;
		}

		std::string_view get_data_member_info_name(uint16_t idx) const {
			const uint32_t offset = sizeof(DataPacketLayoutDataHead) + idx * sizeof(DataPacketLayoutStructureMemberRecord);
			constexpr uint32_t _offset2 = offsetof(DataPacketLayoutStructureMemberRecord, name_offset);
			if (const auto name_offset = _desc_segment->at<uint32_t>(offset + _offset2); name_offset != nullptr) {
				if(*name_offset == std::numeric_limits<uint32_t>::max()) { return std::string_view(); }
				return std::string_view(_desc_segment->at<const char>(*name_offset));
			}
			return std::string_view();
		}

		std::string_view get_data_member_info_desc_text(uint16_t idx) const {
			const uint32_t offset = sizeof(DataPacketLayoutDataHead) + idx * sizeof(DataPacketLayoutStructureMemberRecord);
			constexpr uint32_t _offset2 = offsetof(DataPacketLayoutStructureMemberRecord, desc_offset);
			if (const auto desc_text_offset = _desc_segment->at<uint32_t>(offset + _offset2); desc_text_offset != nullptr) {
				if (*desc_text_offset == std::numeric_limits<uint32_t>::max()) { return std::string_view(); }
				return std::string_view(_desc_segment->at<const char>(*desc_text_offset));
			}
			return std::string_view();
		}

		MVector<uint32_t> get_data_member_info_array_info(uint16_t idx) const {
			const uint16_t array_dimension = get_data_memeber_info_array_dimension(idx);
			if (array_dimension == 0) return {};
			const uint32_t offset = sizeof(DataPacketLayoutDataHead) + idx * sizeof(DataPacketLayoutStructureMemberRecord);
			constexpr uint32_t _offset2 = offsetof(DataPacketLayoutStructureMemberRecord, array_info_offset);
			if (const auto array_info_offset = _desc_segment->at<uint32_t>(offset + _offset2); array_info_offset != nullptr) {
				if (*array_info_offset == -1) { return {}; }
				MVector<uint32_t> result{ array_dimension };
				
				const uint32_t* array_ptr = _desc_segment->at<uint32_t>(*array_info_offset);

				memcpy(result.data(), array_ptr, array_dimension * sizeof(uint32_t));
				return result;
			}
			return {};
		}

		uint64_t get_data_member_info_size(uint16_t idx) const {
			const uint32_t offset = sizeof(DataPacketLayoutDataHead) + idx * sizeof(DataPacketLayoutStructureMemberRecord);
			constexpr uint32_t _offset2 = offsetof(DataPacketLayoutStructureMemberRecord, data_size);
			if (const auto member_size = _desc_segment->at<uint64_t>(offset + _offset2); member_size != nullptr) {
				return *member_size;
			}
			return 0;
		}

		uint32_t get_data_member_info_custom_data(uint16_t idx) const {
			const uint32_t offset = sizeof(DataPacketLayoutDataHead) + idx * sizeof(DataPacketLayoutStructureMemberRecord);
			constexpr uint32_t _offset2 = offsetof(DataPacketLayoutStructureMemberRecord, custom_data);
			if (const auto member_size = _desc_segment->at<uint32_t>(offset + _offset2); member_size != nullptr) {
				return *member_size;
			}
			return 0;
		}

		inline uint64_t get_segment_size_data() const { return _data_segment_size; }

		inline uint32_t get_segment_size_desc() const { return _desc_segment->get_used_memory_size(); }

	private:
		observer_ptr<DataPacketDataStructureDescriptor> _target_desc{};
		std::unique_ptr<DataBufferVector> _desc_segment{};
		uint64_t _data_segment_size{};
	};

	class DataPacket : public Inherit<DataPacket, Object> {
	public:
		DataPacket() = delete;

		DataPacket(DataPacketLayout* layout) {
			_layout = layout;
			if(layout == nullptr) {
				MString msg = MString::format("DataPacket::DataPacket: layout is not vaild");
				error(msg);
				throw Exception{ msg, 0 };
			}
		}

		bool compile() {
			if (!_layout.is_object_vaild()) {
				MString msg = MString::format("DataPacket::compile: layout is not vaild");
				error(msg);
				return false;
			}

			_desc_segment_size = _layout->get_segment_size_desc();
			_data_segment_size = _layout->get_segment_size_data();

			_desc_segment_size = _layout->get_segment_size_desc();
			_data_segment_size = _layout->get_segment_size_data();

			_data_segment.resize(_data_segment_size);

			return true;
		}

		~DataPacket() override = default;

	private:
		observer_ptr<DataPacketLayout> _layout{};
		DataBuffer _data_segment{};
		uint32_t _desc_segment_size{};
		uint64_t _data_segment_size{};
		MVector<DataBuffer> _ptr_datas{};
	};

	/*class DataPacketVisitor : public Inherit<DataPacketVisitor, Object> {
	public:

	};*/

	//class DataPacketVector :

}
