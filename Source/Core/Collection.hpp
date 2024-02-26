#pragma once

#include "mimalloc.h"
#undef max
#undef min
//#include "emhash/hash_set4.hpp"
#include "emhash/hash_set2.hpp"
//#include "emhash/hash_set3.hpp"
//#include "emhash/hash_set8.hpp"

#include "phmap/phmap.h"
#include "ankerlmap/unordered_dense.h"
#include "Hash/xxh3.h"

#include <filesystem>
#include <vector>
#include <string>
#include <format>
#include <cstdint>
#include <ranges>
#include <sstream>



template<class T, class T2> using MMap = ankerl::unordered_dense::map<T, T2>;

//template<class T, class T2> using MMapForInterget = ankerl::unordered_dense::map<T, T2>;

template<class T, class T2, class T3 = phmap::Less<T>> using MBTreeMap = phmap::btree_map<T, T2, T3>;

template<class T> using MSet = ankerl::unordered_dense::set<T, ankerl::unordered_dense::hash<T>>;

template<class T> using MSet_InstertAndAccess_Int_Int_NO_ITOR = emhash2::HashSet<T, ankerl::unordered_dense::hash<T>>;

template<class T> using MBTreeSet = phmap::btree_set<T>;

template<class T> using MBTreeMutiSet = phmap::btree_multiset<T>;

template<class T, class T2> using MBTreeMutiMap = phmap::btree_multimap<T, T2>;

template<class T, size_t block_size = 4096> using MChunkVector = ankerl::unordered_dense::segmented_vector<T, std::allocator<T>, block_size>;

template<class T>
class MVector : public std::vector<T> {
public:
	MVector() : std::vector<T>() {}
	MVector(std::initializer_list<T> il) : std::vector<T>(il) {}

	template<class ...Args>
	MVector(Args&& ...args) : std::vector<T>(std::forward<Args>(args)...) {}

	uint8_t size_u8() const {
		return static_cast<uint8_t>(this->size());
	}

	uint16_t size_u16() const {
		return static_cast<uint16_t>(this->size());
	}

	uint32_t size_u32() const {
		return static_cast<uint32_t>(this->size());
	}
	
	friend size_t hash_value(const MVector<T>& p) {
		const XXH64_hash_t hash = XXH64(p.data(), p.size() * sizeof(T), 0);
		return static_cast<size_t>(hash);
	}
};

class MString;

using MStringList = MVector<MString>;

using MStringStream = std::stringstream;

class MString : public std::string {
public:
	template<class ...Args>
	MString(Args&& ...args) : std::string(std::forward<Args>(args)...) {}

	inline MStringList split(std::string_view delim) const noexcept  {
		MStringList result{};
		result.reserve(size() / 3 * 2);

		for (auto w : std::ranges::views::split(*this, delim)) {
			result.emplace_back(w.begin(), w.end());
		}
		return result;
	}

	inline MString replace_str(std::string_view from, std::string_view to) const {
		MString result = *this;
		size_t start_pos = 0;
		while ((start_pos = result.find(from, start_pos)) != std::string::npos) {
			result.replace(start_pos, from.length(), to);
			start_pos += to.length();
		}
		return result;
	}

	template<class ...ARGS>
	static MString format(const std::string_view str, ARGS&& ...args) {
		MString result;
		result.reserve(str.size());
		std::vformat_to(std::back_inserter(result), str, std::make_format_args(args...));
		return result;
	}

	friend size_t hash_value(const MString& p) {
		const XXH64_hash_t hash = XXH64(p.c_str(), p.size(), 0);
		return static_cast<size_t>(hash);
	}

	std::optional<std::pair<MString, MString>> pick_string_segment(std::string_view from, std::string_view to) const {
		MString copy = *this;
		auto begin = copy.find(from);
		if (begin == MString::npos) { return std::nullopt; }

		auto end = copy.find(to, begin);
		if (end == MString::npos) { return std::nullopt; }

		MString segment = substr(begin, end - begin);
		copy.erase(begin, end - begin);
		return std::make_pair(copy, segment);
	}
};

template <>
struct ankerl::unordered_dense::hash<MString> {
	using is_avalanching = void;

	[[nodiscard]] auto operator()(MString const& p) const noexcept -> uint64_t {
		const XXH64_hash_t hash = XXH64(p.c_str(), p.size(), 0);
		return static_cast<size_t>(hash);
	}
};

//template <>
//struct ankerl::unordered_dense::hash<std::pair<uint32_t, uint32_t>> {
//	using is_avalanching = void;
//
//	[[nodiscard]] auto operator()(std::pair<uint32_t, uint32_t> const& p) const noexcept -> uint64_t {
//		return std::bit_cast<uint64_t>(p);
//	}
//};

template <typename T>
struct ankerl::unordered_dense::hash<MVector<T>> {
	using is_avalanching = void;

	[[nodiscard]] auto operator()(MVector<T> const& p) const noexcept -> uint64_t {
		return hash_value(p);
	}
};

template <>
struct std::formatter<MString>{
	constexpr auto parse(std::format_parse_context& ctx) {
		return ctx.begin();
	}

	template <typename FormatContext> auto format(const MString& str, FormatContext& ctx) const {
		return std::format_to(ctx.out(), "{}", str.c_str());
	}
};