//
// Copyright(c) 2016-2017 benikabocha.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)
//

#include "PMDModel.h"
#include "PMDFile.h"
#include "MMDPhysics.h"

#include <libMMD/Base/Path.h>
#include <libMMD/Base/File.h>
#include <libMMD/Base/Log.h>

#include <Eigen/Core>
#include <Eigen/Geometry>
#include <limits>
#include <algorithm>

namespace libmmd
{
	namespace
	{
		std::string ResolveToonTexturePath(
			const std::string& mmdDataDir,
			const std::string& mmdLoadDir,
			const std::string& texName
		)
		{
			std::string filepath = PathUtil::Combine(mmdLoadDir, texName);

			File file;
			if (file.Open(filepath))
			{
				return filepath;
			}

			filepath = PathUtil::Combine(mmdDataDir, texName);
			if (file.Open(filepath))
			{
				return filepath;
			}

			LIBMMD_WARN("Toon Texture File Not Found. [{}]", texName);
			return "";
		}
	}

	PMDModel::~PMDModel()
	{
		m_materials.clear();
		m_subMeshes.clear();
		m_nodeMan.GetNodes()->clear();
		m_morphMan.GetMorphs()->clear();
	}

	void PMDModel::InitializeAnimation()
	{
		ClearBaseAnimation();

		for (const auto& node : *m_nodeMan.GetNodes())
		{
			node->SetAnimationTranslate(Eigen::Vector3f::Zero());
			node->SetAnimationRotate(Eigen::Quaternionf::Identity());
		}

		for (const auto& node : *m_nodeMan.GetNodes())
		{
			node->UpdateLocalTransform();
		}

		for (const auto& morph : *m_morphMan.GetMorphs())
		{
			morph->SetWeight(0);
		}

		for (const auto& node : *m_nodeMan.GetNodes())
		{
			if (node->GetParent() == nullptr)
			{
				node->UpdateGlobalTransform();
			}
		}

		for (const auto& solver : *m_ikSolverMan.GetIKSolvers())
		{
			solver->Enable(true);
			solver->Solve();
		}

		ResetPhysics();

		const auto rigidbodys = m_physicsMan.GetRigidBodys();
		for (const auto& rb : *rigidbodys)
		{
			rb->SetActivation(true);
		}
	}

	void PMDModel::BeginAnimation()
	{
		for (const auto& node : *m_nodeMan.GetNodes())
		{
			node->BeginUpdateTransform();
		}
	}

	void PMDModel::EndAnimation()
	{
		for (const auto& node : *m_nodeMan.GetNodes())
		{
			node->EndUpdateTransform();
		}
	}

	void PMDModel::UpdateNodeAnimation(const bool afterPhysicsAnim)
	{
		if (afterPhysicsAnim)
		{
			return;
		}

		for (const auto& node : *m_nodeMan.GetNodes())
		{
			node->UpdateLocalTransform();
		}

		for (const auto& node : *m_nodeMan.GetNodes())
		{
			if (node->GetParent() == nullptr)
			{
				node->UpdateGlobalTransform();
			}
		}

		for (const auto& solver : *m_ikSolverMan.GetIKSolvers())
		{
			solver->Solve();
		}
	}

	void PMDModel::ResetPhysics()
	{
		MMDPhysicsManager* physicsMan = GetPhysicsManager();
		const auto physics = physicsMan->GetMMDPhysics();

		if (physics == nullptr)
		{
			return;
		}

		const auto rigidbodys = physicsMan->GetRigidBodys();
		for (const auto& rb : *rigidbodys)
		{
			rb->SetActivation(false);
			rb->ResetTransform();
		}

		physics->Update(1.0f / 60.0f);

		for (const auto& rb : *rigidbodys)
		{
			rb->ReflectGlobalTransform();
		}
		for (const auto& rb : *rigidbodys)
		{
			rb->CalcLocalTransform();
		}

		for (const auto& node : *m_nodeMan.GetNodes())
		{
			if (node->GetParent() == nullptr)
			{
				node->UpdateGlobalTransform();
			}
		}

		for (const auto& rb : *rigidbodys)
		{
			rb->Reset(physics);
		}
	}

	void PMDModel::UpdatePhysicsAnimation(const float elapsed)
	{
		MMDPhysicsManager* physicsMan = GetPhysicsManager();
		const auto physics = physicsMan->GetMMDPhysics();

		if (physics == nullptr)
		{
			return;
		}

		const auto rigidbodys = physicsMan->GetRigidBodys();
		for (const auto& rb : *rigidbodys)
		{
			rb->SyncBonePositionToPhysics(elapsed);
		}

		physics->Update(elapsed);

		for (const auto& rb : *rigidbodys)
		{
			rb->ReflectGlobalTransform();
			rb->CalcLocalTransform();
		}

		for (const auto& node : *m_nodeMan.GetNodes())
		{
			if (node->GetParent() == nullptr)
			{
				node->UpdateGlobalTransform();
			}
		}
	}

	bool PMDModel::Load(const std::string& filepath, const std::string& mmdDataDir)
	{
		Destroy();

		PMDFile pmd;
		if (!ReadPMDFile(&pmd, filepath.c_str()))
		{
			return false;
		}

		if (!LoadPMD(pmd, PathUtil::GetDirectoryName(filepath), mmdDataDir))
		{
			return false;
		}

		return true;
	}

	bool PMDModel::LoadPMD(const PMDFile& file, const std::string& dirPath, const std::string& mmdDataDir)
	{
		m_modelName = file.m_header.m_modelName.ToUtf8String();
		m_comment = file.m_header.m_comment.ToUtf8String();
		if(file.m_header.m_haveEnglishNameExt)
		{
			m_englishModelName = file.m_header.m_englishModelNameExt.ToUtf8String();

			m_englishComment = file.m_header.m_englishCommentExt.ToUtf8String();
		}

		std::vector<std::string> toonTextures;
		toonTextures.reserve(file.m_toonTextureNames.size());
		for (const auto& toonTexName : file.m_toonTextureNames)
		{
			toonTextures.emplace_back(
				ResolveToonTexturePath(
				mmdDataDir,
				dirPath,
				toonTexName.ToUtf8String()
				));
		}

		// Material
		m_materials.reserve(file.m_materials.size());
		m_subMeshes.reserve(file.m_materials.size());
		uint32_t beginIndex = 0;
		for (const auto& [m_diffuse, m_alpha, m_specularPower, m_specular, m_ambient, m_toonIndex, m_edgeFlag, m_faceVertexCount, m_textureName] : file.m_materials)
		{
			MMDMaterial mat;
			mat.m_diffuse = m_diffuse;
			mat.m_alpha = m_alpha;
			mat.m_specularPower = m_specularPower;
			mat.m_specular = m_specular;
			mat.m_ambient = m_ambient;
			mat.m_edgeFlag = m_edgeFlag;
			mat.m_edgeSize = m_edgeFlag == 0 ? 0.0f : 1.0f;
			mat.m_spTextureMode = MMDMaterial::SphereTextureMode::None;
			mat.m_bothFace = false;

			std::string orgTexName = m_textureName.ToUtf8String();
			std::string texName;
			std::string spTexName;
			if (auto asterPos = orgTexName.find_first_of('*'); asterPos == std::string::npos)
			{
				if (std::string ext = PathUtil::GetExt(orgTexName); ext == "sph")
				{
					spTexName = orgTexName;
					mat.m_spTextureMode = MMDMaterial::SphereTextureMode::Mul;
				}
				else if (ext == "spa")
				{
					spTexName = orgTexName;
					mat.m_spTextureMode = MMDMaterial::SphereTextureMode::Add;
				}
				else
				{
					texName = orgTexName;
				}
			}
			else
			{
				texName = orgTexName.substr(0, asterPos);
				spTexName = orgTexName.substr(asterPos + 1);
				if (std::string ext = PathUtil::GetExt(spTexName); ext == "sph")
				{
					mat.m_spTextureMode = MMDMaterial::SphereTextureMode::Mul;
				}
				else if (ext == "spa")
				{
					mat.m_spTextureMode = MMDMaterial::SphereTextureMode::Add;
				}
			}

			if (!texName.empty())
			{
				std::string texPath = PathUtil::Combine(dirPath, texName);
				mat.m_texture = PathUtil::Normalize(texPath);
			}

			if (!spTexName.empty())
			{
				std::string spTexPath = PathUtil::Combine(dirPath, spTexName);
				mat.m_spTexture = PathUtil::Normalize(spTexPath);
			}

			if (m_toonIndex != 255)
			{
				mat.m_toonTexture = toonTextures[m_toonIndex];
			}

			m_materials.emplace_back(mat);
			m_subMeshes.emplace_back(static_cast<int>(beginIndex),
				static_cast<int>(m_faceVertexCount),
				static_cast<int>(m_materials.size() - 1));

			beginIndex = beginIndex + m_faceVertexCount;
		}

		// Node
		m_nodeMan.GetNodes()->reserve(file.m_bones.size());
		for (const auto& bone : file.m_bones)
		{
			auto* node = m_nodeMan.AddNode();
			node->SetName(bone.m_boneName.ToUtf8String());
		}
		for (size_t i = 0; i < file.m_bones.size(); i++)
		{
			const auto& [m_boneName, m_parent, m_tail, m_boneType, m_ikParent, m_position, m_englishBoneNameExt] = file.m_bones[i];
			auto* node = m_nodeMan.GetNode(i);
			if (m_parent != 0xFFFF)
			{
				const auto& parentBone = file.m_bones[m_parent];
				auto* parentNode = m_nodeMan.GetNode(m_parent);
				parentNode->AddChild(node);
				Eigen::Vector3f localPos = m_position - parentBone.m_position;
				node->SetTranslate(localPos);
			}
			else
			{
				Eigen::Vector3f localPos = m_position;
				node->SetTranslate(localPos);
			}
			Eigen::Matrix4f init = Eigen::Matrix4f::Identity();
			init.block<3,1>(0,3) = m_position;
			node->SetGlobalTransform(init);
			node->CalculateInverseInitTransform();
			node->SaveInitialTRS();
		}

		// IK
		m_ikSolverMan.GetIKSolvers()->reserve(file.m_iks.size());
		for (const auto& [m_ikNode, m_ikTarget, m_numChain, m_numIteration, m_rotateLimit, m_chanins] : file.m_iks)
		{
			auto solver = m_ikSolverMan.AddIKSolver();
			auto* ikNode = m_nodeMan.GetNode(m_ikNode);
			solver->SetIKNode(ikNode);

			auto* targetNode = m_nodeMan.GetNode(m_ikTarget);
			solver->SetTargetNode(targetNode);

			for (const auto& chain : m_chanins)
			{
				auto* chainNode = m_nodeMan.GetNode(chain);
				auto findPos = chainNode->GetName().find(u8"ひざ");
				bool isKnee = false;
				if (findPos != std::string::npos)
				{
					isKnee = true;
				}
				solver->AddIKChain(chainNode, isKnee);
				chainNode->EnableIK(true);
			}

			solver->SetIterateCount(m_numIteration);
			solver->SetLimitAngle(m_rotateLimit * 4.0f);
			solver->BuildChainPath();
		}

		LoadMorph(file);

		if (!m_physicsMan.Create())
		{
			LIBMMD_ERROR("Create Physics Fail.");
			return false;
		}

		for (const auto& pmdRB : file.m_rigidBodies)
		{
			auto rb = m_physicsMan.AddRigidBody();
			MMDNode* node = nullptr;
			if (pmdRB.m_boneIndex != 0xFFFF)
			{
				node = m_nodeMan.GetMMDNode(pmdRB.m_boneIndex);
			}
			if (!rb->Create(pmdRB, this, node))
			{
				LIBMMD_ERROR("Create Rigid Body Fail.\n");
				return false;
			}
			m_physicsMan.GetMMDPhysics()->AddRigidBody(rb);
		}

		for (const auto& pmdJoint : file.m_joints)
		{
			if (pmdJoint.m_rigidBodyA != -1 &&
				pmdJoint.m_rigidBodyB != -1 &&
				pmdJoint.m_rigidBodyA != pmdJoint.m_rigidBodyB)
			{
				auto joint = m_physicsMan.AddJoint();
				auto rigidBodys = m_physicsMan.GetRigidBodys();
				bool ret = joint->CreateJoint(
					pmdJoint,
					(*rigidBodys)[pmdJoint.m_rigidBodyA].get(),
					(*rigidBodys)[pmdJoint.m_rigidBodyB].get()
				);
				if (!ret)
				{
					LIBMMD_ERROR("Create Joint Fail.\n");
					return false;
				}
				m_physicsMan.GetMMDPhysics()->AddJoint(joint);
			}
			else
			{
				LIBMMD_WARN("Illegal Joint [{}]", pmdJoint.m_jointName.ToUtf8String());
			}
		}

		ResetPhysics();

		return true;
	}

	void PMDModel::Destroy()
	{
		m_materials.clear();
		m_subMeshes.clear();
		m_nodeMan.GetNodes()->clear();
		m_morphMan.GetMorphs()->clear();
	}

	PMDModel::MorphVertex::MorphVertex(const uint32_t index, const Eigen::Vector3f& position): m_index(index)
		, m_position(position)
	{}

	void PMDModel::LoadMorph(const PMDFile& file)
	{
		for (const auto& [m_morphName, m_morphType, m_vertices, m_englishShapeNameExt] : file.m_morphs)
		{
			if (m_morphType != libmmd::PMDMorph::Base)
			{
				PMDMorph* morph = m_morphMan.AddMorph();
				morph->SetName(m_morphName.ToUtf8String());
				morph->SetWeight(0.0f);
			}
		}
	}

}
