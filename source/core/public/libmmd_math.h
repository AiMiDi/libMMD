/**************************************************************************

Copyright:Copyright(c) 2022-present, Aimidi & libmmd contributors.
Author:			Aimidi
Date:			2023/4/11
File:			libmmd_math.hpp
Description:	math util

**************************************************************************/
#pragma once

#include "libmmd_type.h"

namespace libmmd
{
	static constexpr double PI = 3.1415926535897932384626433832795;
	static constexpr double PI05 = 1.5707963267948966192313216916398;

	/**
	 * \brief Converts Euler rotation to quaternion rotation.
	 * \param euler Enter Euler rotation
	 * \return Results Quaternion rotation
	 */
	template<class T>
	Vector4<T> euler_to_quaternion(const Vector3<T>& euler)
	{
		// pitch(H), roll(-P), yaw(Z)
		const double sin_x = sin(static_cast<double>(-euler[1]) * 0.5);
		const double sin_y = sin(static_cast<double>(euler[0]) * 0.5);
		const double sin_z = sin(static_cast<double>(euler[2]) * 0.5);
		const double cos_x = cos(static_cast<double>(-euler[1]) * 0.5);
		const double cos_y = cos(static_cast<double>(euler[0]) * 0.5);
		const double cos_z = cos(static_cast<double>(euler[2]) * 0.5);
		return {safe_convert<T>(cos_z* cos_y* sin_x + sin_z * sin_y * cos_x), // x
				safe_convert<T>(sin_z* cos_y* sin_x - cos_z * sin_y * cos_x), // y
				safe_convert<T>(cos_z* sin_y* sin_x - sin_z * cos_y * cos_x), // z
				safe_convert<T>(cos_z* cos_y* cos_x + sin_z * sin_y * sin_x)};// w
	}
	/**
	 * \brief Converts Quaternion rotation to euler rotation.
	 * \tparam T Vector float type
	 * \param quaternion Enter quaternion rotation
	 * \return Results euler rotation
	 */
	template<class T>
	Vector3<T> quaternion_to_euler(const Vector4<T>& quaternion)
	{
		// pitch(y - axis rotation)
		const double sin_roll_cos_pitch = 2.0 * (static_cast<double>(quaternion[3] * quaternion[1]) +
			static_cast<double>(quaternion[0] * quaternion[2]));
		const double cos_roll_cos_pitch = 1.0 - 2.0 * (static_cast<double>(quaternion[0] * quaternion[0]) +
			static_cast<double>(quaternion[1] * quaternion[1]));
		const double pitch = -atan2(sin_roll_cos_pitch, cos_roll_cos_pitch);

		// yaw(z - axis rotation)
		const double sin_yaw_cos_pitch = 2.0 * (static_cast<double>(-quaternion[3] * quaternion[2]) -
			static_cast<double>(quaternion[0] * quaternion[1]));
		const double cos_yaw_cos_pitch = 1.0 - 2.0 * (static_cast<double>(quaternion[0] * quaternion[0]) +
			static_cast<double>(quaternion[2] * quaternion[2]));
		const double yaw = atan2(sin_yaw_cos_pitch, cos_yaw_cos_pitch);

		// roll(x - axis rotation)
		double roll;
		if (const double sin_pitch = 2.0 * (static_cast<double>(quaternion[2] * quaternion[1]) -
			static_cast<double>(quaternion[3] *quaternion[0])); sin_pitch >= 1.0)
		{
			// use 90 degrees if out of range
			roll = -PI05;
		}
		else if (sin_pitch <= -1.0)
		{
			roll = PI05;
		}
		else {
			roll = -asin(sin_pitch);
		}

		// fixing the x rotation, part 1
		if (quaternion[0] * quaternion[0] > 0.5f || quaternion[3] < 0.0f)
		{
			if (quaternion[0] < 0.0f)
			{
				roll = -PI - roll;
			}
			else
			{
				roll = PI * copysign(1.0, quaternion[3]) - roll;
			}
		}
		// fixing the x rotation, part 2
		if (roll > PI05)
		{
			roll = PI - roll;
		}
		else if (roll < -PI05)
		{
			roll = -PI - roll;
		}
		// HPB
		return {safe_convert<T>(pitch), // x
				safe_convert<T>(-roll), // y
				safe_convert<T>(yaw) }; // z
	}
}
