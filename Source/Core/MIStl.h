#pragma once

#include "mimalloc.h"
#include "parallel_hashmap/phmap.h"

#include <vector>
#include <stack>
#include <string>

template<class T> using MDeque = std::deque<T, mi_stl_allocator<T>>;

template<class T> using MStack = std::stack<T, MDeque<T>>;

template<class T, class T2> using MMap = phmap::flat_hash_map<T, T2, phmap::priv::hash_default_hash<T>, phmap::priv::hash_default_eq<T>, mi_stl_allocator<T>>;

template<class T, class T2> using MBTreeMap = phmap::btree_map<T, T2, phmap::Less<T>, mi_stl_allocator<T>>;

template<class T> using MSet = phmap::flat_hash_set<T, phmap::priv::hash_default_hash<T>, phmap::priv::hash_default_eq<T>, mi_stl_allocator<T>>;

template<class T> using MBTreeSet = phmap::btree_set<T, phmap::Less<T>, mi_stl_allocator<T>>;

template<class T> using MBTreeMutiSet = phmap::btree_multiset<T, phmap::Less<T>, mi_stl_allocator<T>>;

template<class T, class T2> using MBTreeMutiMap = phmap::btree_multimap<T, T2, phmap::Less<T>, mi_stl_allocator<T>>;

template<class T> using MVector = std::vector<T, mi_stl_allocator<T>>;

using MString = std::basic_string<char, std::char_traits<char>, mi_stl_allocator<char>>;


inline std::string to_stdstring(const MString& other)  {
	return std::string(other.begin(), other.end());
}

inline MString to_MString(const std::string& other) {
	return MString(other.begin(), other.end());
}

template<class T> std::vector<T> to_vector(const MVector<T>& other) {
	return std::vector<T>(other.begin(), other.end());
}

template<class T> MVector<T> to_MVector(const std::vector<T>& other) {
	return MVector<T>(other.begin(), other.end());
}
