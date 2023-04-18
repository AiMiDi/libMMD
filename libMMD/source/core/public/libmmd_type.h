/**************************************************************************

Copyright:Copyright(c) 2022-present, Aimidi & libMMD contributors.
Author:			Aimidi
Date:			2023/4/10
File:			libmmd_type.hpp
Description:	type 

**************************************************************************/

#pragma once

#include "libmmd_marco.h"

namespace libmmd
{
	using Char = char;				// signed 8 bit character
	using UChar = unsigned char;	// unsigned 8 bit character

	using Int16 = int16_t;			// 16 bit signed integer data type.
	using UInt16 = uint16_t;		// 16 bit unsigned integer data type.
	using Int32 = int32_t;			// 32 bit signed integer data type.
	using UInt32 = uint32_t;		// 32 bit unsigned integer data type.
	using Int64 = int64_t;			// 64 bit signed integer data type.
	using UInt64 = uint64_t;		// 64 bit unsigned integer data type.

	using Float32 = float;			// 32 bit floating point value (float)
	using Float64 = double;			// 64 bit floating point value (double)

	template <typename T>
	using Vector2 = std::array<T, 2>;
	template <typename T>
	using Vector3 = std::array<T, 3>;
	template <typename T>
	using Vector4 = std::array<T, 4>;

	using Vector2d32 = Vector2<Float32>;
	using Vector2d64 = Vector2<Float64>;
	using Vector32   = Vector3<Float32>;
	using Vector64   = Vector3<Float64>;
	using Vector4d32 = Vector4<Float32>;
	using Vector4d64 = Vector4<Float64>;

	// Safely converts a 64/32 bit float value into another scalar value.
	// The resulting value will be clipped against its boundaries, without raising an exception.
	template <typename T, typename U>
	LIBMMD_ATTRIBUTE_FORCE_INLINE T safe_convert(U src)
	{
		if (src >= static_cast<U>(std::numeric_limits<T>::max()))
			return std::numeric_limits<T>::max();
		if (src <= static_cast<U>(std::numeric_limits<T>::min()))
			return std::numeric_limits<T>::min();
		return static_cast<T>(src);
	}

	static_assert(sizeof(Vector4d32) == 16);
}
