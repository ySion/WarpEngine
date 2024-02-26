#pragma once

#include "DataBuffer.hpp"

WARP_TYPE_NAME_2(Data, DataBufferString);

namespace Warp::Data {

	class DataBufferString : public InheritPure {
	public:

		DataBufferString() = default;

		~DataBufferString() override = default;

		DataBufferString(std::string_view str);

		DataBufferString(DataBufferString&& other) noexcept;

		DataBufferString& operator=(DataBufferString&& other) noexcept;

		const char* write_string(std::string_view str);

		inline const char* read_string() const {
			return reinterpret_cast<const char*>(_string_buffer.data());
		}

		inline std::string_view read_string_view() const {
			return { reinterpret_cast<const char*>(_string_buffer.data()), _string_len };
		}

		inline uint64_t length() const {
			return _string_len;
		}

		inline void reserve(uint64_t size) {
			_string_buffer.resize(size);
		}

		inline void clear_with_memory() {
			_string_len = 0;
			_string_buffer.clear_with_memory();
		}

	private:
		uint64_t _string_len{};
		DataBuffer _string_buffer{};
	};

}