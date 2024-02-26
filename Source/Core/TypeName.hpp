#pragma once
#include <string>
#include <typeinfo>
#include <stdint.h>

namespace Warp {

	template<class T>
	constexpr const char* type_name() noexcept { return typeid(T).name(); }

	template<> constexpr const char* type_name<std::string>() noexcept { return "std::string"; }
	template<> constexpr const char* type_name<uint8_t>() noexcept { return "uint8_t"; }
	template<> constexpr const char* type_name<uint16_t>() noexcept { return "uint16_t"; }
	template<> constexpr const char* type_name<uint32_t>() noexcept { return "uint32_t"; }
	template<> constexpr const char* type_name<uint64_t>() noexcept { return "uint64_t"; }
	template<> constexpr const char* type_name<int8_t>() noexcept { return  "int8_t"; }
	template<> constexpr const char* type_name<int16_t>() noexcept { return "int16_t"; }
	template<> constexpr const char* type_name<int32_t>() noexcept { return "int32_t"; }
	template<> constexpr const char* type_name<int64_t>() noexcept { return "int64_t"; }
	template<> constexpr const char* type_name<float>() noexcept { return "float"; }
	template<> constexpr const char* type_name<double>() noexcept { return "dobule"; }
	

	template<class T>
		requires std::is_integral_v<T>
	inline uint64_t to_u64(T val) {
		return static_cast<uint64_t>(val);

	}
	template<class T>
		requires std::is_integral_v<T>
	inline uint32_t to_u32(T val) {
		return static_cast<uint32_t>(val);
	}

	template<class T>
		requires std::is_integral_v<T>
	inline uint16_t to_u16(T val) {
		return static_cast<uint16_t>(val);
	}

	template<class T>
		requires std::is_integral_v<T>
	inline uint8_t to_u8(T val) {
		return static_cast<uint8_t>(val);
	}
}

#define WARP_TYPE_NAME(T) \
	namespace Warp{class T;} \
	template<> constexpr const char* ::Warp::type_name<::Warp::T>() noexcept { return #T; }

#define WARP_TYPE_NAME_2(T, T2) \
	namespace Warp::T{class T2;} \
	template<> constexpr const char* ::Warp::type_name<::Warp::T::T2>() noexcept { return #T"::"#T2; }

#define WARP_TYPE_NAME_3(T, T2, T3) \
	namespace Warp::T::T2{class T3;} \
	template<> constexpr const char* ::Warp::type_name<::Warp::T::T2::T3>() noexcept { return  #T"::"#T2"::"#T3; }


