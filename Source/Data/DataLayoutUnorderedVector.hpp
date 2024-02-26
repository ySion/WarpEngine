#pragma once

#include "Core/Core.hpp"
#include "DataLayout.hpp"
#include "DataBufferVector.hpp"
#include "DataBufferUnorderedUniformVector.hpp"
namespace Warp::Data {
	class DataLayoutUnorderedVector {
	public:
		DataLayoutUnorderedVector() = delete;

		DataLayoutUnorderedVector(DataLayout* p_layout) {
			if (observer_ptr check{ p_layout }; check.is_object_vaild()) {
				_layout = p_layout;
				_element_size = p_layout->get_data_segment_size();
			}
		}

		~DataLayoutUnorderedVector() = default;

		DataLayoutUnorderedVector(const DataLayoutUnorderedVector&) = delete;

		DataLayoutUnorderedVector& operator=(const DataLayoutUnorderedVector&) = delete;

		DataLayoutUnorderedVector(DataLayoutUnorderedVector&&) = default;

		DataLayoutUnorderedVector& operator=(DataLayoutUnorderedVector&&) = default;

		bool push_back(const void* p) {
			return _data.push_back(p, _element_size);
		}

		bool push_back_empty(uint64_t count) {
			return _data.push_back_empty(_element_size * count);
		}

		bool pop_back() {
			return _data.pop_back(_element_size);
		}

		bool fetch_back(void* p) {
			return _data.fetch_back(p, _element_size);
		}

		template<class T> bool fetch_back(T& element) {
			if (sizeof(T) != _element_size) {
				MString msg = MString::format("DataLayoutUnorderedVector::fetch_back: element size not match, element size: {}, uniform element size: {}", sizeof(T), _element_size);
				error(msg);
				return false;
			}
			return _data.fetch_back(&element, _element_size);
		}

		uint64_t size() const {
			return _data.size() / _element_size;
		}

		void* at(uint64_t index) const {
			return _data.at(index * _element_size);
		}

		template<class T> T* at(uint64_t index) {
			return static_cast<T*>(_data.at(index * _element_size));
		}

		bool is_index_vaild(uint64_t index) const {
			return index < size();
		}

		uint64_t size_used_memory() const {
			return _data.size();
		}

		uint64_t size_total_memory() const {
			return _data.total_size();
		}

		bool erase(uint64_t index) {
			if(!_data.move(size() - 1, index, _element_size)){
				return false;
			}
			return pop_back();
		}

	private:
		uint64_t _element_size{ 1 };
		DataLayout* _layout{};
		DataBufferVector _data{};
	};




}