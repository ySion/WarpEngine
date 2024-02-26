#pragma once

#include "Core/Core.hpp"
#include "DataBufferUnorderedUniformVector.hpp"

WARP_TYPE_NAME_2(Data, DataLayoutUnorderedBlockVector);

namespace Warp::Data {

	class DataLayout;
	class DataBufferString;
	/// 注意: 请低频率使用 text(string) 和 data_block, 由于内存不连续性, 所以它们会导致显著的性能下降
	/// 以结构体形式访问方法: struct *p = get_data_root_address(uint64_t element_idx = 0);
	/// 此时若需要访问字符串成员, 请使用 const char* ptr
	/// 访问注意事项:
	/// 非指针类型可以直接修改, 指针类型需要通过set_data_text(对于字符串)或者set_data_block(对于数据块)来修改, 所有指针类型需要使用const来限定
	/// 它在增加时可以保证顺序
	/// 删除的时候会打乱顺序
	/// 它在删除的时候会打乱元素的顺序
	class DataLayoutUnorderedBlockVector : public Inherit<DataLayoutUnorderedBlockVector, Object> {
	public:
		DataLayoutUnorderedBlockVector() = delete;

		~DataLayoutUnorderedBlockVector() override = default;

		DataLayoutUnorderedBlockVector(DataLayout* layout);

		bool push_back(uint64_t element_count = 0);

		bool erase(uint64_t element_idx);

		uint64_t back() const;

		bool pop_back();

		void* at(uint64_t element_idx) const;

		uint64_t count() const { return _data_segment.count(); }

		uint64_t size_used_memory() const { return _data_segment.size_used_memory(); }

		uint64_t size_total_memory() const { return _data_segment.size_total_memory(); }

		/// 通过名字获取数据
		/// 非指针类型可以直接修改, 指针类型需要通过set_data_text(对于字符串)或者set_data_block(对于数据块)来修改, 所有指针类型需要使用const来限定
		void* get_data_start_address(const MString& name, uint64_t element_idx) const;

		bool set_data_document(const MString& name, uint64_t element_idx, std::string_view str) const;

		bool set_data_block(const MString& name, uint64_t element_idx, void* p, uint64_t size) const;

		DataLayout* get_layout() const;

		template<class T> T* at(uint64_t element_idx = 0) const { return static_cast<T*>(_data_segment.at(element_idx)); }

		template<class T> T* get_data_start_address(const MString& name, uint64_t element_idx) const { return static_cast<T*>(get_data_start_address(name, element_idx)); }

	private:
		bool move(uint64_t A, uint64_t B); // 移动覆盖

		bool compile();

		DataLayout* _layout{};
		DataBufferUnorderedUniformVector _data_segment{};
		MVector<std::unique_ptr<DataBufferString>> _data_segment_document{};
		MVector<std::unique_ptr<DataBufferShrinkBuffer>> _data_segment_ptr{};
	};

}
