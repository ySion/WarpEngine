#pragma once

#include "DataBufferUnorderedUniformVector.hpp"
#include "Core/Core.hpp"
#include "DataLayout.hpp"
#include "DataBufferVector.hpp"

namespace Warp::Data{

	template<size_t PageSize = 8192 * 2, size_t Alignment = 4096>
	class DataLayoutUnorderedUniformChunkVector {
		struct alignas(Alignment) PageData {
			uint8_t _data[PageSize];
		};
	public:
	
		DataLayoutUnorderedUniformChunkVector() = delete;

		~DataLayoutUnorderedUniformChunkVector() = default;

		//选填layout和element_size, 如果layout不为空, element_size将被忽略
		DataLayoutUnorderedUniformChunkVector(DataLayout* layout, uint32_t element_size) {

			if(layout == nullptr && element_size != 0){
				_layout = nullptr;
				_element_size = element_size;
			}else {
				if (observer_ptr check{ layout }; !check.is_object_vaild()) {
					throw Exception("DataLayoutUnorderedUniformChunkVector::DataLayoutUnorderedUniformChunkVector: layout is invaild.");
				}

				if (!layout->is_pure()) {
					throw Exception("DataLayoutUnorderedUniformChunkVector::DataLayoutUnorderedUniformChunkVector: layout is not pure.");
				}

				_layout = layout;
				_element_size = layout->get_data_segment_size();
			}

			_element_count_per_page = PageSize / _element_size;
		}

		DataLayoutUnorderedUniformChunkVector(DataLayoutUnorderedUniformChunkVector&&) = default;

		DataLayoutUnorderedUniformChunkVector& operator=(DataLayoutUnorderedUniformChunkVector&&) = default;

		DataLayoutUnorderedUniformChunkVector(const DataLayoutUnorderedUniformChunkVector&) = delete;

		DataLayoutUnorderedUniformChunkVector& operator=(const DataLayoutUnorderedUniformChunkVector&) = delete;

		void push_back(const void* p) {
			if (p == nullptr) return;
			try_adjust_space(1);
			auto idx = map_index(_count);
			memcpy(&(_page[idx.first]->_data[idx.second * element_size()]), p, _element_size);
			_count++;
		}

		void* at(uint32_t index) const {
			if (index > _count) {
				#if defined WARP_DEBUG
				MString msg = MString::format("DataLayoutUnorderedUniformChunkVector::at: index out of range, index: {}, count: {}", index, _count);
				error(msg);
				#endif
				return nullptr;
			}
			auto idx = map_index(index);
			return &(_page[idx.first]->_data[idx.second * element_size()]);
		}

		template<class T> T* at(uint32_t index) {
			if (index > _count) return nullptr;
			auto idx = map_index(index);
			return (T*)(&(_page[idx.first]->_data[idx.second * element_size()]));
		}

		template<class T> void push_back_pure(const T& val) {
			if (sizeof(T) != _element_size) {
				error("DataLayoutUnorderedUniformChunkVector::push_back_pure: element size not match, size {}.", sizeof(T));
				return;
			}

			try_adjust_space(1);
			auto idx = map_index(_count);
			*(T*)(&(_page[idx.first]->_data[idx.second * element_size()])) = val;
			_count++;
		}

		bool pop_back() {
			if (_count == 0) return false;
			_count--;
			return true;
		}

		bool erase(uint32_t index) {
			if (index >= _count) return false;
			if (!move_overlay(_count - 1, index)) return false;
			_count--;
			return true;
		}

		bool move_overlay(uint32_t from, uint32_t to) const{ 
			if (from == to) { return true; }
			if (from >= _count || to >= _count) { return false; }
			auto idx_1 = map_index(from);
			auto idx_2 = map_index(to);
			memcpy(_page[idx_2.first].at(idx_2.second), _page[idx_1.first].at(idx_1.second), _element_size);
			return true;
		}

		void clear() {
			_count = 0;
		}

		void push_back_empty(uint32_t new_count) {
			reserve(count() + new_count);
			_count = count() + new_count;
		}

		void resize(uint32_t new_count) {
			if (new_count <= count()) return;
			reserve(new_count);
			_count = new_count;
		}

		void reserve(uint32_t size) {
			if (size <= _capacity) return;
			try_adjust_space(size - _capacity);
		}

		inline uint32_t count() const { return _count; }

		inline uint32_t page_count() const { return _page.size_u32(); }

		inline void shrink_to_fit() {
			auto page_count_after_shrink = (_count + _element_count_per_page - 1) / _element_count_per_page;
			if (page_count_after_shrink == _page.size()) return;
			_page.resize(page_count_after_shrink);
			_capacity = page_count_after_shrink * _element_count_per_page;
			_page.shrink_to_fit();
		}

		inline uint32_t capacity() const { return _capacity; }

		inline uint32_t element_size() const { return _element_size; }

		static constexpr auto page_size() {
			return PageSize;
		}

		static constexpr auto alignment() {
			return Alignment;
		}

	private:

		std::pair<uint32_t, uint32_t> map_index(uint32_t index) const {
			return{ index / _element_count_per_page, index % _element_count_per_page };
		}

		void fetch_next_idx_vec_for_new_items(uint32_t count, MVector<uint32_t>& idx_vec, uint32_t& new_count_required) {
			idx_vec.resize(count);
			new_count_required = count;
			for (uint32_t i = 0; i < count; i++) {
				idx_vec[i] = _count;
				_count++;
			}
		}

		void new_page(const uint32_t count) {
			
			_page.reserve(_page.size() + count);

			for(uint32_t i = 0; i < count; i++) {
				auto& ref = _page.emplace_back(new PageData{});
			}
			_capacity += count * _element_count_per_page;
		}

		void try_adjust_space(uint32_t new_count) {
			if (_count + new_count <= _capacity) {
				return;
			}

			uint32_t need_count = (_count + new_count) - _capacity;
			uint32_t need_page = (need_count + _element_count_per_page - 1) / _element_count_per_page;
			new_page(need_page);
		}

		uint32_t _count {};
		uint32_t _capacity {};
		uint32_t _element_size{};

		DataLayout* _layout{};
		uint32_t _element_count_per_page{};

		MVector <std::unique_ptr<PageData>> _page{};
	};

}