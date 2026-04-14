//
// Copyright(c) 2016-2019 benikabocha.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)
//

#ifndef LIBMMD_MODEL_MMD_VMDINTERPOLATION_H_
#define LIBMMD_MODEL_MMD_VMDINTERPOLATION_H_

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <vector>

#include <Eigen/Geometry>

namespace libmmd
{
	struct VMDMotion;

	/**
	 * @brief Represents a Bezier curve used in VMD animation.
	 */
	struct VMDBezier
	{
		/**
		 * @brief Evaluate the X component of the Bezier curve at time t.
		 * @param t The time parameter.
		 * @return The evaluated X component.
		 */
		float EvalX(float t) const;

		/**
		 * @brief Evaluate the Y component of the Bezier curve at time t.
		 * @param t The time parameter.
		 * @return The evaluated Y component.
		 */
		float EvalY(float t) const;

		/**
		 * @brief Evaluate both X and Y components of the Bezier curve at time t.
		 * @param t The time parameter.
		 * @return The evaluated X and Y components as a Eigen::Vector2f.
		 */
		Eigen::Vector2f Eval(float t) const;

		/**
		 * @brief Evaluate the derivative of the X component at time t.
		 * @param t The time parameter.
		 * @return dX/dt at the given time.
		 */
		float EvalDX(float t) const;

		/**
		 * @brief Find the parameter t such that EvalX(t) ≈ time, using Newton-Raphson.
		 * @param time The target X value in [0, 1].
		 * @return The parameter t corresponding to the given X value.
		 */
		float FindBezierX(float time) const;

		Eigen::Vector2f	m_cp1 = Eigen::Vector2f::Zero(); ///< Control point 1
		Eigen::Vector2f	m_cp2 = Eigen::Vector2f::Zero(); ///< Control point 2
	};

	/**
	 * @brief Decode VMD interpolation bytes into normalized bezier control points.
	 * @param bezier Output bezier.
	 * @param cp Pointer to the VMD interpolation bytes for one channel.
	 */
	void SetVMDBezier(VMDBezier& bezier, const uint8_t* cp);

	/**
	 * @brief Encode normalized bezier control points into VMD interpolation bytes.
	 * @param bezier Input bezier.
	 * @param cp Output interpolation bytes for one channel.
	 */
	void GetVMDBezier(const VMDBezier& bezier, uint8_t* cp);

	/**
	 * @brief Bone keyframe data used for standalone VMD interpolation.
	 */
	struct VMDBoneKeyframe
	{
		int32_t				frame = 0;
		Eigen::Vector3f		translate = Eigen::Vector3f::Zero();
		Eigen::Quaternionf	rotate = Eigen::Quaternionf::Identity();
		VMDBezier			txBezier;
		VMDBezier			tyBezier;
		VMDBezier			tzBezier;
		VMDBezier			rotBezier;

		VMDBoneKeyframe() = default;
		explicit VMDBoneKeyframe(const VMDMotion& motion);

		void Set(const VMDMotion& motion);
	};

	/**
	 * @brief Interpolated transform produced from two VMD bone keys.
	 */
	struct VMDBoneInterpolationResult
	{
		Eigen::Vector3f		translate = Eigen::Vector3f::Zero();
		Eigen::Quaternionf	rotate = Eigen::Quaternionf::Identity();
	};

	namespace detail
	{
		template <typename KeyType>
		auto GetKeyTimeImpl(const KeyType& key, int) -> decltype(key.frame)
		{
			return key.frame;
		}

		template <typename KeyType>
		auto GetKeyTimeImpl(const KeyType& key, long) -> decltype(key.m_time)
		{
			return key.m_time;
		}

		template <typename KeyType>
		auto GetKeyTime(const KeyType& key) -> decltype(GetKeyTimeImpl(key, 0))
		{
			return GetKeyTimeImpl(key, 0);
		}
	}

	/**
	 * @brief Find the bound key in a vector of keys.
	 * @tparam KeyType Type of the key.
	 * @param keys Vector of keys.
	 * @param t Time to find the bound key for.
	 * @param startIdx Starting index for the search.
	 * @return Iterator to the first key with time > t.
	 */
	template <typename KeyType>
	typename std::vector<KeyType>::const_iterator FindBoundKey(
		const std::vector<KeyType>&	keys,
		int32_t						t,
		size_t						startIdx = 0
	) {
		if (keys.empty() || keys.size() <= startIdx)
		{
			return keys.end();
		}

		const auto key0 = detail::GetKeyTime(keys[startIdx]);
		if (key0 <= t)
		{
			if (startIdx + 1 < keys.size())
			{
				const auto key1 = detail::GetKeyTime(keys[startIdx + 1]);
				if (key1 > t)
				{
					return keys.begin() + static_cast<typename std::vector<KeyType>::difference_type>(startIdx + 1);
				}
			}
			else
			{
				return keys.end();
			}
		}
		else
		{
			if (startIdx != 0)
			{
				const auto key1 = detail::GetKeyTime(keys[startIdx - 1]);
				if (key1 <= t)
				{
					return keys.begin() + static_cast<typename std::vector<KeyType>::difference_type>(startIdx);
				}
			}
			else
			{
				return keys.begin();
			}
		}

		auto boundIt = std::upper_bound(keys.begin(), keys.end(), t, [](int32_t lhs, const KeyType& rhs) {
			return lhs < detail::GetKeyTime(rhs);
		});
		return boundIt;
	}

	/**
	 * @brief Interpolate between two VMD bone keys.
	 * @param key0 Start key.
	 * @param key1 End key.
	 * @param currentFrame Current frame to evaluate.
	 * @return Interpolated translation and rotation.
	 */
	VMDBoneInterpolationResult InterpolateBoneKeys(
		const VMDBoneKeyframe&	key0,
		const VMDBoneKeyframe&	key1,
		float					currentFrame
	);
}

#endif // !LIBMMD_MODEL_MMD_VMDINTERPOLATION_H_
