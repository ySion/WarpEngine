#include "Core/Utils.h"

#include <fstream>
#include <string>
#include <vector>

#include "MIStl.h"

namespace Warp
{

	MString read_text_file(const MString& filename) {
		MVector<MString> data;

		std::ifstream file;

		file.open(filename.c_str(), std::ios::in);

		if (!file.is_open()) {
			throw std::exception(std::format("file {} open failed", filename).c_str());
		}

		MString str = MString{ (std::istreambuf_iterator<char>(file)),
			(std::istreambuf_iterator<char>()) };

		return std::move(str);
	}

	MVector<uint8_t> read_binary_file(const MString& filename, const uint32_t count) {
		MVector<uint8_t> data;

		std::ifstream file;

		file.open(filename.c_str(), std::ios::in | std::ios::binary);

		if (!file.is_open()) {
			throw std::exception(std::format("file {} open failed", filename).c_str());
		}

		uint64_t read_count = count;
		if (count == 0) {
			file.seekg(0, std::ios::end);
			read_count = static_cast<uint64_t>(file.tellg());
			file.seekg(0, std::ios::beg);
		}

		data.resize(static_cast<size_t>(read_count));
		file.read(reinterpret_cast<char*>(data.data()), read_count);
		file.close();

		return std::move(data);
	}

}
