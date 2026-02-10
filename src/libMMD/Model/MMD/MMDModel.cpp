//
// Copyright(c) 2016-2017 benikabocha.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)
//

#include "MMDModel.h"
#include "MMDPhysics.h"
#include "VPDFile.h"
#include "VMDAnimation.h"

#include <Eigen/Core>
#include <Eigen/Geometry>

#include <libMMD/Base/Log.h>

namespace libmmd
{
	MMDPhysicsManager::MMDPhysicsManager() = default;

	MMDPhysicsManager::~MMDPhysicsManager()
	{
		for (auto& joint : m_joints)
		{
			m_mmdPhysics->RemoveJoint(joint.get());
		}
		m_joints.clear();

		for (auto& rb : m_rigidBodys)
		{
			m_mmdPhysics->RemoveRigidBody(rb.get());
		}
		m_rigidBodys.clear();

		m_mmdPhysics.reset();
	}

	bool MMDPhysicsManager::Create()
	{
		m_mmdPhysics = std::make_unique<MMDPhysics>();
		return m_mmdPhysics->Create();
	}

	MMDPhysics* MMDPhysicsManager::GetMMDPhysics() const
	{
		return m_mmdPhysics.get();
	}

	MMDRigidBody* MMDPhysicsManager::AddRigidBody()
	{
		LIBMMD_ASSERT(m_mmdPhysics != nullptr);
		return m_rigidBodys.emplace_back(std::make_unique<MMDRigidBody>()).get();
	}

	MMDJoint* MMDPhysicsManager::AddJoint()
	{
		LIBMMD_ASSERT(m_mmdPhysics != nullptr);
		return m_joints.emplace_back(std::make_unique<MMDJoint>()).get();
	}

	void MMDModel::SaveBaseAnimation()
	{
		const auto nodeMan = GetNodeManager();
		for (size_t i = 0; i < nodeMan->GetNodeCount(); i++)
		{
			const auto node = nodeMan->GetMMDNode(i);
			node->SaveBaseAnimation();
		}

		const auto morphMan = GetMorphManager();
		for (size_t i = 0; i < morphMan->GetMorphCount(); i++)
		{
			const auto morph = morphMan->GetMorph(i);
			morph->SaveBaseAnimation();
		}

		const auto ikMan = GetIKManager();
		for (size_t i = 0; i < ikMan->GetIKSolverCount(); i++)
		{
			const auto ikSolver = ikMan->GetMMDIKSolver(i);
			ikSolver->SaveBaseAnimation();
		}
	}

	void MMDModel::LoadBaseAnimation()
	{
		const auto nodeMan = GetNodeManager();
		for (size_t i = 0; i < nodeMan->GetNodeCount(); i++)
		{
			const auto node = nodeMan->GetMMDNode(i);
			node->LoadBaseAnimation();
		}

		const auto morphMan = GetMorphManager();
		for (size_t i = 0; i < morphMan->GetMorphCount(); i++)
		{
			const auto morph = morphMan->GetMorph(i);
			morph->LoadBaseAnimation();
		}

		const auto ikMan = GetIKManager();
		for (size_t i = 0; i < ikMan->GetIKSolverCount(); i++)
		{
			const auto ikSolver = ikMan->GetMMDIKSolver(i);
			ikSolver->LoadBaseAnimation();
		}
	}

	void MMDModel::ClearBaseAnimation()
	{
		const auto nodeMan = GetNodeManager();
		for (size_t i = 0; i < nodeMan->GetNodeCount(); i++)
		{
			const auto node = nodeMan->GetMMDNode(i);
			node->ClearBaseAnimation();
		}

		const auto morphMan = GetMorphManager();
		for (size_t i = 0; i < morphMan->GetMorphCount(); i++)
		{
			const auto morph = morphMan->GetMorph(i);
			morph->ClearBaseAnimation();
		}

		const auto ikMan = GetIKManager();
		for (size_t i = 0; i < ikMan->GetIKSolverCount(); i++)
		{
			const auto ikSolver = ikMan->GetMMDIKSolver(i);
			ikSolver->ClearBaseAnimation();
		}
	}

	void MMDModel::UpdateAllAnimation(const VMDAnimation * vmdAnim, const float vmdFrame, const float physicsElapsed)
	{
		if (vmdAnim != nullptr)
		{
			vmdAnim->Evaluate(vmdFrame);
		}

		UpdateMorphAnimation();

		UpdateNodeAnimation(false);

		UpdatePhysicsAnimation(physicsElapsed);

		UpdateNodeAnimation(true);
	}

	void MMDModel::LoadPose(const VPDFile & vpd, const int frameCount)
	{
		struct Pose
		{
			MMDNode*	m_node;
			Eigen::Vector3f	m_beginTranslate;
			Eigen::Vector3f	m_endTranslate;
			Eigen::Quaternionf	m_beginRotate;
			Eigen::Quaternionf	m_endRotate;

			explicit Pose(
				MMDNode* node = nullptr,
				const Eigen::Vector3f& beginTranslate = Eigen::Vector3f::Zero(),
				const Eigen::Vector3f& endTranslate = Eigen::Vector3f::Zero(),
				const Eigen::Quaternionf& beginRotate = Eigen::Quaternionf::Identity(),
				const Eigen::Quaternionf& endRotate = Eigen::Quaternionf::Identity()
			)
				: m_node(node),
				  m_beginTranslate(beginTranslate),
				  m_endTranslate(endTranslate),
				  m_beginRotate(beginRotate),
				  m_endRotate(endRotate)
			{
			}
		};
		std::vector<Pose> poses;
		for (const auto& bone : vpd.m_bones)
		{
			if (const auto nodeIdx = GetNodeManager()->FindNodeIndex(bone.m_boneName); MMDNodeManager::NPos != nodeIdx)
			{
				const auto node = GetNodeManager()->GetMMDNode(bone.m_boneName);
				poses.emplace_back(node,
					node->GetAnimationTranslate(),
					bone.m_translate,
					node->GetAnimationRotate(),
					bone.m_quaternion);
			}
		}

		struct Morph
		{
			MMDMorph*	m_morph;
			float		m_beginWeight;
			float		m_endWeight;

			explicit Morph(
				MMDMorph* morph = nullptr,
				const float beginWeight = 0.0f,
				const float endWeight = 0.0f
			)
				: m_morph(morph),
				  m_beginWeight(beginWeight),
				  m_endWeight(endWeight)
			{
			}
		};
		std::vector<Morph> morphs;
		for (const auto& [m_morphName, m_weight] : vpd.m_morphs)
		{
			if (const auto morphIdx = GetMorphManager()->FindMorphIndex(m_morphName); MMDMorphManager::NPos != morphIdx)
			{
				const auto morph = GetMorphManager()->GetMorph(m_morphName);
				morphs.emplace_back(morph, morph->GetWeight(), m_weight);
			}
		}

		// Apply Physics
		for (int i = 0; i < frameCount; i++)
		{
			BeginAnimation();

			// evaluate
			float w = static_cast<float>(1 + i) / static_cast<float>(frameCount);
			for (auto& pose : poses)
			{
				Eigen::Vector3f t = pose.m_beginTranslate + (pose.m_endTranslate - pose.m_beginTranslate) * w;
				auto q = pose.m_beginRotate.slerp(w, pose.m_endRotate);
				pose.m_node->SetAnimationTranslate(t);
				pose.m_node->SetAnimationRotate(q);
			}

			for (auto& morph : morphs)
			{
				auto weight = morph.m_beginWeight + (morph.m_endWeight - morph.m_beginWeight) * w;
				morph.m_morph->SetWeight(weight);
			}

			UpdateMorphAnimation();
			UpdateNodeAnimation(false);
			UpdatePhysicsAnimation(1.0f / 30.0f);
			UpdateNodeAnimation(true);

			EndAnimation();
		}

		//SaveBaseAnimation();
	}
}
