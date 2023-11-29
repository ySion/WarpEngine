#pragma once

#include "mimalloc.h"
#include "parallel_hashmap/phmap.h"

#include <vector>
#include <stack>
#include <string>
#include <format>

template<class T> using MDeque = std::deque<T, mi_stl_allocator<T>>;

//template<class T> using MStack = std::stack<T, MDeque<T>>;

template<class T, class T2, class T3 = phmap::priv::hash_default_hash<T>, class T4 = phmap::priv::hash_default_eq<T>> using MMap = phmap::flat_hash_map<T, T2, T3, T4, mi_stl_allocator<T>>;

template<class T, class T2, class T3 = phmap::Less<T>> using MBTreeMap = phmap::btree_map<T, T2, T3, mi_stl_allocator<T>>;

template<class T, class T2 = phmap::priv::hash_default_hash<T>, class T3 = phmap::priv::hash_default_eq<T>> using MSet = phmap::flat_hash_set<T, T2, T3, mi_stl_allocator<T>>;

template<class T> using MBTreeSet = phmap::btree_set<T, phmap::Less<T>, mi_stl_allocator<T>>;

template<class T> using MBTreeMutiSet = phmap::btree_multiset<T, phmap::Less<T>, mi_stl_allocator<T>>;

template<class T, class T2> using MBTreeMutiMap = phmap::btree_multimap<T, T2, phmap::Less<T>, mi_stl_allocator<T>>;

template<class T> using MVector = std::vector<T, mi_stl_allocator<T>>;

using MString = std::basic_string<char, std::char_traits<char>, mi_stl_allocator<char>>;

template<class T> class MStack {
	MVector<T> m_data{};
	size_t stack_top = 0;
public:

	MStack(size_t reserve_size = 64) {
		m_data.reserve(reserve_size);
	}

	inline void push(const T& item) {
		if (m_data.size() <= stack_top) {
			m_data.push_back(item);
		} else {
			m_data[stack_top] = item;
		}
		stack_top++;
	}

	constexpr T pop() {
		if (stack_top <= 0) { return T{}; }
		stack_top--;
		return m_data[stack_top];
	}

	constexpr bool empty() const {
		return stack_top <= 0;
	}

	constexpr T top() {
		if (stack_top > 0) { return m_data[stack_top - 1]; }
		return T{};
	}

	constexpr size_t size() const {
		return stack_top;
	}

	constexpr void clear() {
		stack_top = 0;
		m_data.clear();
	}

	constexpr void reserve(size_t size) {
		m_data.reserve(size);
	}
};

constexpr MVector<MString> mstring_split(const MString& str, const MString& delim) {
	MVector<MString> result{};
	result.reserve(str.size() / 3 * 2);
	//size_t start = 0;
	//size_t end = str.find(delim);
	//while (end != MString::npos) {
	//	result.push_back(str.substr(start, end - start));
	//	start = end + delim.length();
	//	end = str.find(delim, start);
	//}
	//result.push_back(str.substr(start, end));

	for (auto w : std::ranges::views::split(str, delim)) {
		result.emplace_back(w.begin(), w.end());
	}
	return result;
}

constexpr MVector<MString> mstring_split(const MString& str, const char delim) {
	MVector<MString> result{};
	result.reserve(str.size() / 3 * 2);
	/*size_t start = 0;
	size_t end = str.find(delim);
	while (end != MString::npos) {
		result.push_back(str.substr(start, end - start));
		start = end + 1;
		end = str.find(delim, start);
	}
	result.push_back(str.substr(start, end));*/

	for(auto w : std::ranges::views::split(str, delim)) {
		result.emplace_back(w.begin(), w.end());
	}

	return result;
}


inline std::string to_stdstring(const MString& other)  {
	return std::string(other.begin(), other.end());
}

inline MString to_MString(const std::string& other) {
	return MString(other.begin(), other.end());
}

template<class ...ARGS>
constexpr MString mformat(const std::string_view ctx, ARGS&& ...arg) {
	std::string temp = vformat(ctx, std::make_format_args(std::forward<ARGS>(arg)...));
	return to_MString(temp);
}

template<class T> std::vector<T> to_vector(const MVector<T>& other) {
	return std::vector<T>(other.begin(), other.end());
}

template<class T> MVector<T> to_MVector(const std::vector<T>& other) {
	return MVector<T>(other.begin(), other.end());
}
