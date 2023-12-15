#pragma once

#include "mimalloc.h"
#include "parallel_hashmap/phmap.h"

#include <vector>
#include <stack>
#include <string>
#include <format>

template<class T, class T2, class T3 = phmap::priv::hash_default_hash<T>, class T4 = phmap::priv::hash_default_eq<T>> using mmap = phmap::flat_hash_map<T, T2, T3, T4, mi_stl_allocator<T>>;

template<class T, class T2 = phmap::priv::hash_default_hash<T>, class T3 = phmap::priv::hash_default_eq<T>> using mset = phmap::flat_hash_set<T, T2, T3, mi_stl_allocator<T>>;

template<class T, class T2, class T3 = phmap::priv::hash_default_hash<T>, class T4 = phmap::priv::hash_default_eq<T>> using MMap = phmap::flat_hash_map<T, T2, T3, T4, mi_stl_allocator<T>>;

template<class T, class T2, class T3 = phmap::Less<T>> using MBTreeMap = phmap::btree_map<T, T2, T3, mi_stl_allocator<T>>;

template<class T, class T2 = phmap::priv::hash_default_hash<T>, class T3 = phmap::priv::hash_default_eq<T>> using MSet = phmap::flat_hash_set<T, T2, T3, mi_stl_allocator<T>>;

template<class T> using MBTreeSet = phmap::btree_set<T, phmap::Less<T>, mi_stl_allocator<T>>;

template<class T> using MBTreeMutiSet = phmap::btree_multiset<T, phmap::Less<T>, mi_stl_allocator<T>>;

template<class T, class T2> using MBTreeMutiMap = phmap::btree_multimap<T, T2, phmap::Less<T>, mi_stl_allocator<T>>;


template<class T>
class MVector : public std::vector<T, mi_stl_allocator<T>> {
public:
	template<class ...Args>
	MVector(Args&& ...args) : std::vector<T, mi_stl_allocator<T>>(std::forward<Args>(args)...) {}

	operator std::vector<T>() {
		return std::vector(this->begin(), this->end());
	}

	constexpr uint32_t size_u32() const {
		return static_cast<uint32_t>(std::vector<T, mi_stl_allocator<T>>::size());
	}
};

class MString;

using MStringList = MVector<MString>;


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
};


template<class T> class mstack {
	MVector<T> m_data{};
	size_t stack_top = 0;
public:

	mstack(size_t reserve_size = 64) {
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