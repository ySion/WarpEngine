#include "DataLayoutUnorderedBlockVector.hpp"
#include "DataBufferString.hpp"
#include "DataLayout.hpp"
#include "Core/observer_ptr.hpp"

using namespace Warp::Data;

DataLayoutUnorderedBlockVector::DataLayoutUnorderedBlockVector(DataLayout* layout) {
	_layout = layout;
	compile();
}

bool DataLayoutUnorderedBlockVector::compile() {
	if (const observer_ptr check{ _layout }; !check.is_object_vaild()) {
		MString msg = MString::format("DataLayoutUnorderedBlockVector::compile: layout is not vaild");
		error(msg);
		return false;
	}
	_data_segment.init(_layout->get_data_segment_size());
	return true;
}

bool DataLayoutUnorderedBlockVector::push_back(uint64_t element_count) {

	if (const observer_ptr check{ _layout }; !check.is_object_vaild()) {
		MString msg = MString::format("DataLayoutUnorderedBlockVector::push_back: layout is not vaild");
		error(msg);
		return false;
	}

	if (!_data_segment.push_back_empty(element_count)) {
		MString msg = MString::format("DataLayoutUnorderedBlockVector::push_back: push_back_empty failed");
		error(msg);
		return false;
	}

	const uint32_t doc_count = _layout->get_memeber_document_count();
	const uint32_t ptr_count = _layout->get_memeber_ptr_count();

	if (doc_count == 0 && ptr_count == 0) { return true; }

	_data_segment_document.reserve(_data_segment_document.size() + doc_count * element_count);
	_data_segment_ptr.reserve(_data_segment_ptr.size() + doc_count * element_count);

	for (uint32_t i = 0; i < doc_count * element_count; i++) {
		_data_segment_document.emplace_back(new DataBufferString());
	}

	for (uint32_t i = 0; i < ptr_count * element_count; i++) {
		_data_segment_ptr.emplace_back(new DataBufferShrinkBuffer());
	}

	return true;
}

bool DataLayoutUnorderedBlockVector::erase(uint64_t element_idx) {
	if (!move(back(), element_idx)) {
		return false;
	}
	return _data_segment.pop_back();
}

uint64_t DataLayoutUnorderedBlockVector::back() const {
	return _data_segment.count() - 1;
}

bool DataLayoutUnorderedBlockVector::pop_back() {
	return _data_segment.pop_back();
}

bool DataLayoutUnorderedBlockVector::move(uint64_t A, uint64_t B) {
	if (A == B) return true;
	if(count() <= A || count() <= B) {
		return false;
	}

	if (!_data_segment.move(A, B)) {
		return false;
	}

	const uint32_t doc_count = _layout->get_memeber_document_count();
	const uint32_t ptr_count = _layout->get_memeber_ptr_count();

	//swap buffers:
	const auto all_doc_count = doc_count * count();
	const auto all_ptr_count = ptr_count * count();

	if(all_doc_count > 0) {
		const auto offset_doc_A = doc_count * A;
		const auto offset_doc_B = doc_count * B;

		for(size_t iA = offset_doc_A, iB = offset_doc_B; iA < offset_doc_A + doc_count; iA++, iB++) {
			_data_segment_document[iB] = std::move(_data_segment_document[iA]);
		}
	}

	if(all_ptr_count > 0) {
		const auto offset_ptr_A = ptr_count * A;
		const auto offset_ptr_B = ptr_count * B;

		for (size_t iA = offset_ptr_A, iB = offset_ptr_B; iA < offset_ptr_A + ptr_count; iA++, iB++) {
			_data_segment_ptr[iB] = std::move(_data_segment_ptr[iA]);
		}
	}

	return true;
}

void* DataLayoutUnorderedBlockVector::at(uint64_t element_idx) const {
	return _data_segment.at(element_idx);
}

void* DataLayoutUnorderedBlockVector::get_data_start_address(const MString& name, uint64_t element_idx) const {

	const auto address_offset_warp = _layout->get_member_data_start_address_offset_by_name(name);
	if (!address_offset_warp) return nullptr;
	const auto address_offset = address_offset_warp.value();

	const uint64_t p = ((uint64_t)_data_segment.at(element_idx) + address_offset);
	if (_data_segment.is_address_vailed(p)) { return (void*)p; }
	fatal("DataLayoutUnorderedBlockVector::get_data_start_address: address is not vaild, {} is start, {} is now, {} is end", 0, static_cast<int64_t>(p - reinterpret_cast<uint64_t>(_data_segment.data())), _data_segment.size_used_memory());
	return nullptr;
}

bool DataLayoutUnorderedBlockVector::set_data_document(const MString& name, uint64_t element_idx, std::string_view str) const {

	const char** ptr_ptr = static_cast<const char**>(get_data_start_address(name, element_idx));

	if (ptr_ptr == nullptr) return false;

	const auto linear_idx_warp = _layout->get_member_linear_idx_document_by_name(name);
	if (!linear_idx_warp) return false;
	const auto linear_idx = linear_idx_warp.value();

	const auto buffer_idx = _layout->get_memeber_document_count() * element_idx + linear_idx;
	*ptr_ptr = _data_segment_document.at(buffer_idx)->write_string(str);

	return true;
}

bool DataLayoutUnorderedBlockVector::set_data_block(const MString& name, uint64_t element_idx, void* p, uint64_t size) const {
	void** ptr_ptr = static_cast<void**>(get_data_start_address(name, element_idx));

	if (ptr_ptr == nullptr)  return false;

	const auto linear_idx_warp = _layout->get_member_linear_idx_ptr_by_name(name);
	if (!linear_idx_warp)  return false;
	const auto linear_idx = linear_idx_warp.value();

	const auto buffer_idx = _layout->get_memeber_ptr_count() * element_idx + linear_idx;
	*ptr_ptr = _data_segment_ptr.at(buffer_idx)->write_buffer(p, size);

	return true;
}

DataLayout* DataLayoutUnorderedBlockVector::get_layout() const {
	if (const observer_ptr check{ _layout }; check.is_object_vaild()) { return _layout; }
	return nullptr;
}
