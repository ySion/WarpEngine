#pragma once

#include <filesystem>

#include "mimalloc.h"
#include "parallel_hashmap/phmap.h"

#include <vector>
#include <stack>
#include <string>
#include <format>

template<class T, class T2, class T3 = phmap::priv::hash_default_hash<T>, class T4 = phmap::priv::hash_default_eq<T>> using MMap = phmap::flat_hash_map<T, T2, T3, T4, mi_stl_allocator<T>>;

template<class T, class T2, class T3 = phmap::Less<T>> using MBTreeMap = phmap::btree_map<T, T2, T3, mi_stl_allocator<T>>;

template<class T, class T2 = phmap::priv::hash_default_hash<T>, class T3 = phmap::priv::hash_default_eq<T>> using MSet = phmap::flat_hash_set<T, T2, T3, mi_stl_allocator<T>>;

template<class T> using MBTreeSet = phmap::btree_set<T, phmap::Less<T>, mi_stl_allocator<T>>;

template<class T> using MBTreeMutiSet = phmap::btree_multiset<T, phmap::Less<T>, mi_stl_allocator<T>>;

template<class T, class T2> using MBTreeMutiMap = phmap::btree_multimap<T, T2, phmap::Less<T>, mi_stl_allocator<T>>;



template<class T>
class MVector : public std::vector<T, mi_stl_allocator<T>> {
public:
	MVector() : std::vector<T, mi_stl_allocator<T>>() {}
	MVector(std::initializer_list<T> il) : std::vector<T, mi_stl_allocator<T>>(il) {}

	template<class ...Args>
	MVector(Args&& ...args) : std::vector<T, mi_stl_allocator<T>>(std::forward<Args>(args)...) {}

	uint8_t size_u8() const {
		return static_cast<uint8_t>(this->size());
	}

	uint16_t size_u16() const {
		return static_cast<uint16_t>(this->size());
	}

	uint32_t size_u32() const {
		return static_cast<uint32_t>(this->size());
	}
};

class MString;

using MStringList = MVector<MString>;

using MStringStream = std::basic_stringstream<char, std::char_traits<char>, mi_stl_allocator<char>>;

class MString : public std::basic_string<char, std::char_traits<char>, mi_stl_allocator<char>> {
public:
	template<class ...Args>
	MString(Args&& ...args) : std::basic_string<char, std::char_traits<char>, mi_stl_allocator<char>>(std::forward<Args>(args)...) {}

	inline operator std::string() {
		return std::string(this->begin(), this->end());
	}

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


	inline std::string to_std_string() const {
		return std::string(this->begin(), this->end());
	}

	template<class ...ARGS>
	static MString format(const std::string_view str, ARGS&& ...args) {
		MString result;
		result.reserve(str.size());
		std::vformat_to(std::back_inserter(result), str, std::make_format_args(args...));
		return result;
	}

	friend size_t hash_value(const MString& p) {
		return std::_Hash_array_representation(p.c_str(), p.size());
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