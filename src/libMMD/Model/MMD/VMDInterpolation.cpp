//
// Copyright(c) 2016-2019 benikabocha.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)
//

#include "VMDInterpolation.h"
#include "VMDFile.h"

#include <algorithm>
#include <cmath>

namespace libmmd
{
	void SetVMDBezier(VMDBezier& bezier, const uint8_t* cp)
	{
		const int x0 = cp[0];
		const int y0 = cp[4];
		const int x1 = cp[8];
		const int y1 = cp[12];

		bezier.m_cp1 = Eigen::Vector2f(static_cast<float>(x0) / 127.0f, static_cast<float>(y0) / 127.0f);
		bezier.m_cp2 = Eigen::Vector2f(static_cast<float>(x1) / 127.0f, static_cast<float>(y1) / 127.0f);
	}

	void GetVMDBezier(const VMDBezier& bezier, uint8_t* cp)
	{
		cp[0]  = static_cast<uint8_t>(std::clamp(std::round(bezier.m_cp1.x() * 127.0f), 0.0f, 127.0f));
		cp[4]  = static_cast<uint8_t>(std::clamp(std::round(bezier.m_cp1.y() * 127.0f), 0.0f, 127.0f));
		cp[8]  = static_cast<uint8_t>(std::clamp(std::round(bezier.m_cp2.x() * 127.0f), 0.0f, 127.0f));
		cp[12] = static_cast<uint8_t>(std::clamp(std::round(bezier.m_cp2.y() * 127.0f), 0.0f, 127.0f));
	}

	float VMDBezier::EvalX(const float t) const
	{
		const float t2 = t * t;
		const float t3 = t2 * t;
		const float it = 1.0f - t;
		const float it2 = it * it;
		const float it3 = it2 * it;
		const float x[4] = {
			0.0f,
			m_cp1.x(),
			m_cp2.x(),
			1.0f
		};

		return t3 * x[3] + 3.0f * t2 * it * x[2] + 3.0f * t * it2 * x[1] + it3 * x[0];
	}

	float VMDBezier::EvalY(const float t) const
	{
		const float t2 = t * t;
		const float t3 = t2 * t;
		const float it = 1.0f - t;
		const float it2 = it * it;
		const float it3 = it2 * it;
		const float y[4] = {
			0.0f,
			m_cp1.y(),
			m_cp2.y(),
			1.0f
		};

		return t3 * y[3] + 3.0f * t2 * it * y[2] + 3.0f * t * it2 * y[1] + it3 * y[0];
	}

	Eigen::Vector2f VMDBezier::Eval(const float t) const
	{
		return Eigen::Vector2f(EvalX(t), EvalY(t));
	}

	float VMDBezier::EvalDX(const float t) const
	{
		const float it = 1.0f - t;
		const float x[4] = {
			0.0f,
			m_cp1.x(),
			m_cp2.x(),
			1.0f
		};

		return 3.0f * it * it * (x[1] - x[0])
			 + 6.0f * it * t * (x[2] - x[1])
			 + 3.0f * t * t * (x[3] - x[2]);
	}

	float VMDBezier::FindBezierX(const float time) const
	{
		constexpr float kEpsilon = 0.00001f;

		float t = time;
		for (int i = 0; i < 8; ++i)
		{
			const float x = EvalX(t) - time;
			if (std::abs(x) < kEpsilon)
			{
				return t;
			}

			const float dx = EvalDX(t);
			if (std::abs(dx) < 1e-6f)
			{
				break;
			}

			t -= x / dx;
			t = std::clamp(t, 0.0f, 1.0f);
		}

		if (std::abs(EvalX(t) - time) > kEpsilon)
		{
			float start = 0.0f;
			float stop = 1.0f;
			t = 0.5f;
			float x = EvalX(t);
			while (std::abs(time - x) > kEpsilon)
			{
				if (time < x)
				{
					stop = t;
				}
				else
				{
					start = t;
				}
				t = (stop + start) * 0.5f;
				x = EvalX(t);
			}
		}

		return t;
	}

	VMDBoneKeyframe::VMDBoneKeyframe(const VMDMotion& motion)
	{
		Set(motion);
	}

	void VMDBoneKeyframe::Set(const VMDMotion& motion)
	{
		frame = static_cast<int32_t>(motion.m_frame);
		translate = motion.m_translate;
		rotate = motion.m_quaternion;

		SetVMDBezier(txBezier, &motion.m_interpolation[0]);
		SetVMDBezier(tyBezier, &motion.m_interpolation[1]);
		SetVMDBezier(tzBezier, &motion.m_interpolation[2]);
		SetVMDBezier(rotBezier, &motion.m_interpolation[3]);
	}

	VMDBoneInterpolationResult InterpolateBoneKeys(
		const VMDBoneKeyframe& key0,
		const VMDBoneKeyframe& key1,
		const float currentFrame
	)
	{
		if (currentFrame <= static_cast<float>(key0.frame))
		{
			return { key0.translate, key0.rotate };
		}

		if (currentFrame >= static_cast<float>(key1.frame))
		{
			return { key1.translate, key1.rotate };
		}

		const float timeRange = static_cast<float>(key1.frame - key0.frame);
		if (timeRange <= 0.0f)
		{
			return { key1.translate, key1.rotate };
		}

		const float time = std::clamp(
			(currentFrame - static_cast<float>(key0.frame)) / timeRange,
			0.0f,
			1.0f
		);

		const float txX = key0.txBezier.FindBezierX(time);
		const float tyX = key0.tyBezier.FindBezierX(time);
		const float tzX = key0.tzBezier.FindBezierX(time);
		const float rotX = key0.rotBezier.FindBezierX(time);

		const float txY = key0.txBezier.EvalY(txX);
		const float tyY = key0.tyBezier.EvalY(tyX);
		const float tzY = key0.tzBezier.EvalY(tzX);
		const float rotY = key0.rotBezier.EvalY(rotX);

		VMDBoneInterpolationResult result;
		result.translate = key0.translate + (key1.translate - key0.translate).cwiseProduct(Eigen::Vector3f(txY, tyY, tzY));
		result.rotate = key0.rotate.slerp(rotY, key1.rotate);
		return result;
	}
}
