//
// Copyright(c) 2016-2017 benikabocha.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)
//

#include "MMDIkSolver.h"

#include <algorithm>
#include <cmath>
#include <Eigen/Core>
#include <Eigen/Geometry>
#include <functional>

namespace libmmd
{
	MMDIkSolver::MMDIkSolver()
		: m_ikNode(nullptr)
		, m_ikTarget(nullptr)
		, m_iterateCount(1)
		, m_limitAngle(2.0f * static_cast<float>(EIGEN_PI))
		, m_enable(true)
		, m_baseAnimEnable(true)
	{
	}

	void MMDIkSolver::AddIKChain(MMDNode * node, const bool isKnee)
	{
		m_chains.emplace_back(node,
			isKnee,
			isKnee ? Eigen::Vector3f(0.5f * static_cast<float>(EIGEN_PI) / 180.0f, 0, 0) : Eigen::Vector3f::Zero(),
			isKnee ? Eigen::Vector3f(static_cast<float>(EIGEN_PI), 0, 0) : Eigen::Vector3f::Zero(),
			Eigen::Quaternionf::Identity());
	}

	void MMDIkSolver::AddIKChain(
		MMDNode * node,
		const bool axisLimit,
		const Eigen::Vector3f & limitMin,
		const Eigen::Vector3f & limitMax
	)
	{
		m_chains.emplace_back(node, axisLimit, limitMin, limitMax, Eigen::Quaternionf::Identity());
	}

	void MMDIkSolver::Solve()
	{
		if (!m_enable)
		{
			return;
		}

		if (m_ikNode == nullptr || m_ikTarget == nullptr)
		{
			// wrong ik
			return;
		}

		// Initialize IKChain
		for (auto& chain : m_chains)
		{
			chain.m_prevAngle = Eigen::Vector3f::Zero();
			chain.m_node->SetIKRotate(Eigen::Quaternionf::Identity());
			chain.m_planeModeAngle = 0;

			chain.m_node->UpdateLocalTransform();
			chain.m_node->UpdateGlobalTransform();
		}

		float maxDist = std::numeric_limits<float>::max();
		for (uint32_t i = 0; i < m_iterateCount; i++)
		{
			SolveCore(i);

			Eigen::Vector3f targetPos = m_ikTarget->GetGlobalTransform().col(3).head<3>();
			Eigen::Vector3f ikPos = m_ikNode->GetGlobalTransform().col(3).head<3>();
			const float dist = (targetPos - ikPos).norm();
			if (dist < maxDist)
			{
				maxDist = dist;
				for (auto& chain : m_chains)
				{
					chain.m_saveIKRot = chain.m_node->GetIKRotate();
				}
			}
			else
			{
				for (auto& chain : m_chains)
				{
					chain.m_node->SetIKRotate(chain.m_saveIKRot);
					chain.m_node->UpdateLocalTransform();
					chain.m_node->UpdateGlobalTransform();
				}
				break;
			}
		}
	}

	namespace
	{
		float NormalizeAngle(const float angle)
		{
			constexpr float two_pi = 2.0f * static_cast<float>(EIGEN_PI);
			float ret = angle;
			while (ret >= two_pi)
			{
				ret -= two_pi;
			}
			while (ret < 0)
			{
				ret += two_pi;
			}

			return ret;
		}

		float DiffAngle(const float a, const float b)
		{
			constexpr float pi_f = static_cast<float>(EIGEN_PI);
			constexpr float two_pi = 2.0f * static_cast<float>(EIGEN_PI);
			const float diff = NormalizeAngle(a) - NormalizeAngle(b);
			if (diff > pi_f)
			{
				return diff - two_pi;
			}
			if (diff < -pi_f)
			{
				return diff + two_pi;
			}
			return diff;
		}

		Eigen::Vector3f Decompose(const Eigen::Matrix3f& m, const Eigen::Vector3f& before)
		{
			constexpr float pi_f = static_cast<float>(EIGEN_PI);
			Eigen::Vector3f r;
			float sy = -m(2, 0);
			constexpr float e = 1.0e-6f;
			if (1.0f - std::abs(sy) < e)
			{
				r.y() = std::asin(sy);
				// 180°に近いほうを探す
				float sx = std::sin(before.x());
				float sz = std::sin(before.z());
				if (std::abs(sx) < std::abs(sz))
				{
					// Xのほうが0または180
					float cx = std::cos(before.x());
					if (cx > 0)
					{
						r.x() = 0;
						r.z() = std::asin(-m(0, 1));
					}
					else
					{
						r.x() = pi_f;
						r.z() = std::asin(m(0, 1));
					}
				}
				else
				{
					float cz = std::cos(before.z());
					if (cz > 0)
					{
						r.z() = 0;
						r.x() = std::asin(-m(1, 2));
					}
					else
					{
						r.z() = pi_f;
						r.x() = std::asin(m(1, 2));
					}
				}
			}
			else
			{
				r.x() = std::atan2(m(2, 1), m(2, 2));
				r.y() = std::asin(-m(2, 0));
				r.z() = std::atan2(m(1, 0), m(0, 0));
			}

			Eigen::Vector3f tests[] =
			{
				Eigen::Vector3f(r.x() + pi_f, pi_f - r.y(), r.z() + pi_f),
				Eigen::Vector3f(r.x() + pi_f, pi_f - r.y(), r.z() - pi_f),
				Eigen::Vector3f(r.x() + pi_f, -pi_f - r.y(), r.z() + pi_f),
				Eigen::Vector3f(r.x() + pi_f, -pi_f - r.y(), r.z() - pi_f),
				Eigen::Vector3f(r.x() - pi_f, pi_f - r.y(), r.z() + pi_f),
				Eigen::Vector3f(r.x() - pi_f, pi_f - r.y(), r.z() - pi_f),
				Eigen::Vector3f(r.x() - pi_f, -pi_f - r.y(), r.z() + pi_f),
				Eigen::Vector3f(r.x() - pi_f, -pi_f - r.y(), r.z() - pi_f),
			};

			float errX = std::abs(DiffAngle(r.x(), before.x()));
			float errY = std::abs(DiffAngle(r.y(), before.y()));
			float errZ = std::abs(DiffAngle(r.z(), before.z()));
			float minErr = errX + errY + errZ;
			for (const auto test : tests)
			{
				float err = std::abs(DiffAngle(test.x(), before.x()))
					+ std::abs(DiffAngle(test.y(), before.y()))
					+ std::abs(DiffAngle(test.z(), before.z()));
				if (err < minErr)
				{
					minErr = err;
					r = test;
				}
			}
			return r;
		}
	}

	void MMDIkSolver::SolveCore(uint32_t iteration)
	{
		Eigen::Vector3f ikPos = m_ikNode->GetGlobalTransform().col(3).head<3>();
		//for (auto& chain : m_chains)
		for (size_t chainIdx = 0; chainIdx < m_chains.size(); chainIdx++)
		{
			auto& chain = m_chains[chainIdx];
			MMDNode* chainNode = chain.m_node;
			if (chainNode == m_ikTarget)
			{
				/*
				ターゲットとチェインが同じ場合、 chainTargetVec が0ベクトルとなる。
				その後の計算で求める回転値がnanになるため、計算を行わない
				対象モデル：ぽんぷ長式比叡.pmx
				*/
				continue;
			}

			if (chain.m_enableAxisLimit)
			{
				// X,Y,Z 軸のいずれかしか回転しないものは専用の Solver を使用する
				if ((chain.m_limitMin.x() != 0 || chain.m_limitMax.x() != 0) &&
					(chain.m_limitMin.y() == 0 || chain.m_limitMax.y() == 0) &&
					(chain.m_limitMin.z() == 0 || chain.m_limitMax.z() == 0)
					)
				{
					SolvePlane(iteration, chainIdx, SolveAxis::X);
					continue;
				}
				if ((chain.m_limitMin.y() != 0 || chain.m_limitMax.y() != 0) &&
					(chain.m_limitMin.x() == 0 || chain.m_limitMax.x() == 0) &&
					(chain.m_limitMin.z() == 0 || chain.m_limitMax.z() == 0)
				)
				{
					SolvePlane(iteration, chainIdx, SolveAxis::Y);
					continue;
				}
				if ((chain.m_limitMin.z() != 0 || chain.m_limitMax.z() != 0) &&
					(chain.m_limitMin.x() == 0 || chain.m_limitMax.x() == 0) &&
					(chain.m_limitMin.y() == 0 || chain.m_limitMax.y() == 0)
				)
				{
					SolvePlane(iteration, chainIdx, SolveAxis::Z);
					continue;
				}
			}

			Eigen::Vector3f targetPos = m_ikTarget->GetGlobalTransform().col(3).head<3>();

			Eigen::Matrix4f invChain = chain.m_node->GetGlobalTransform().inverse();

			Eigen::Vector3f chainIkPos = (invChain * Eigen::Vector4f(ikPos.x(), ikPos.y(), ikPos.z(), 1.0f)).head<3>();
			Eigen::Vector3f chainTargetPos = (invChain * Eigen::Vector4f(targetPos.x(), targetPos.y(), targetPos.z(), 1.0f)).head<3>();

			Eigen::Vector3f chainIkVec = chainIkPos.normalized();
			Eigen::Vector3f chainTargetVec = chainTargetPos.normalized();

			float dot = chainTargetVec.dot(chainIkVec);
			dot = std::clamp(dot, -1.0f, 1.0f);

			float angle = std::acos(dot);
			constexpr float pi_f = static_cast<float>(EIGEN_PI);
			float angleDeg = angle * 180.0f / pi_f;
			if (angleDeg < 1.0e-3f)
			{
				continue;
			}
			angle = std::clamp(angle, -m_limitAngle, m_limitAngle);
			Eigen::Vector3f cross = chainTargetVec.cross(chainIkVec).normalized();
			Eigen::Quaternionf rot = Eigen::Quaternionf(Eigen::AngleAxisf(angle, cross));

			Eigen::Quaternionf chainRot = chainNode->GetIKRotate() * chainNode->AnimateRotate() * rot;
			if (chain.m_enableAxisLimit)
			{
				Eigen::Matrix3f chainRotM = chainRot.toRotationMatrix();
				auto rotXYZ = Decompose(chainRotM, chain.m_prevAngle);
				Eigen::Vector3f clampXYZ;
				clampXYZ = rotXYZ.cwiseMax(chain.m_limitMin).cwiseMin(chain.m_limitMax);

				clampXYZ = (clampXYZ - chain.m_prevAngle).cwiseMax(-m_limitAngle).cwiseMin(m_limitAngle) + chain.m_prevAngle;
				Eigen::Quaternionf r = Eigen::Quaternionf(Eigen::AngleAxisf(clampXYZ.x(), Eigen::Vector3f(1, 0, 0)));
				r = r * Eigen::Quaternionf(Eigen::AngleAxisf(clampXYZ.y(), Eigen::Vector3f(0, 1, 0)));
				r = r * Eigen::Quaternionf(Eigen::AngleAxisf(clampXYZ.z(), Eigen::Vector3f(0, 0, 1)));
				chainRotM = r.toRotationMatrix();
				chain.m_prevAngle = clampXYZ;

				chainRot = Eigen::Quaternionf(chainRotM);
			}

			Eigen::Quaternionf ikRot = chainRot * chainNode->AnimateRotate().inverse();
			chainNode->SetIKRotate(ikRot);

			chainNode->UpdateLocalTransform();
			chainNode->UpdateGlobalTransform();
		}
	}

	void MMDIkSolver::SolvePlane(uint32_t iteration, size_t chainIdx, SolveAxis solveAxis)
	{
		int RotateAxisIndex = 0; // X axis
		Eigen::Vector3f RotateAxis = Eigen::Vector3f(1, 0, 0);
		switch (solveAxis)
		{
		case SolveAxis::X:
			RotateAxisIndex = 0; // X axis
			RotateAxis = Eigen::Vector3f(1, 0, 0);
			break;
		case SolveAxis::Y:
			RotateAxisIndex = 1; // Y axis
			RotateAxis = Eigen::Vector3f(0, 1, 0);
			break;
		case SolveAxis::Z:
			RotateAxisIndex = 2; // Z axis
			RotateAxis = Eigen::Vector3f(0, 0, 1);
			break;
		default:
			break;
		}

		auto& chain = m_chains[chainIdx];
		Eigen::Vector3f ikPos = m_ikNode->GetGlobalTransform().col(3).head<3>();

		Eigen::Vector3f targetPos = m_ikTarget->GetGlobalTransform().col(3).head<3>();

		Eigen::Matrix4f invChain = chain.m_node->GetGlobalTransform().inverse();

		Eigen::Vector3f chainIkPos = (invChain * Eigen::Vector4f(ikPos.x(), ikPos.y(), ikPos.z(), 1.0f)).head<3>();
		Eigen::Vector3f chainTargetPos = (invChain * Eigen::Vector4f(targetPos.x(), targetPos.y(), targetPos.z(), 1.0f)).head<3>();

		Eigen::Vector3f chainIkVec = chainIkPos.normalized();
		Eigen::Vector3f chainTargetVec = chainTargetPos.normalized();

		float dot = chainTargetVec.dot(chainIkVec);
		dot = std::clamp(dot, -1.0f, 1.0f);

		float angle = std::acos(dot);

		angle = std::clamp(angle, -m_limitAngle, m_limitAngle);

		Eigen::Quaternionf rot1 = Eigen::Quaternionf(Eigen::AngleAxisf(angle, RotateAxis));
		Eigen::Vector3f targetVec1 = rot1 * chainTargetVec;
		float dot1 = targetVec1.dot(chainIkVec);

		Eigen::Quaternionf rot2 = Eigen::Quaternionf(Eigen::AngleAxisf(-angle, RotateAxis));
		Eigen::Vector3f targetVec2 = rot2 * chainTargetVec;
		float dot2 = targetVec2.dot(chainIkVec);

		float newAngle = chain.m_planeModeAngle;
		if (dot1 > dot2)
		{
			newAngle += angle;
		}
		else
		{
			newAngle -= angle;
		}
		if (iteration == 0)
		{
			if (newAngle < chain.m_limitMin[RotateAxisIndex] || newAngle > chain.m_limitMax[RotateAxisIndex])
			{
				if (-newAngle > chain.m_limitMin[RotateAxisIndex] && -newAngle < chain.m_limitMax[RotateAxisIndex])
				{
					newAngle *= -1;
				}
				else
				{
					auto halfRad = (chain.m_limitMin[RotateAxisIndex] + chain.m_limitMax[RotateAxisIndex]) * 0.5f;
					if (std::abs(halfRad - newAngle) > std::abs(halfRad + newAngle))
					{
						newAngle *= -1;
					}
				}
			}
		}

		newAngle = std::clamp(newAngle, chain.m_limitMin[RotateAxisIndex], chain.m_limitMax[RotateAxisIndex]);
		chain.m_planeModeAngle = newAngle;

		Eigen::Quaternionf ikRotM = Eigen::Quaternionf(Eigen::AngleAxisf(newAngle, RotateAxis)) * chain.m_node->AnimateRotate().inverse();
		chain.m_node->SetIKRotate(ikRotM);

		chain.m_node->UpdateLocalTransform();
		chain.m_node->UpdateGlobalTransform();
	}
}

