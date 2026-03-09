//
// Copyright(c) 2016-2017 benikabocha.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)
//

#include "PMXModel.h"

#include "PMXFile.h"
#include "MMDPhysics.h"

#include <libMMD/Base/Path.h>
#include <libMMD/Base/Log.h>

#include <Eigen/Core>
#include <Eigen/Geometry>
#include <limits>
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <stack>

namespace libmmd
{
	enum class PMXModel::MorphType
	{
		None,
		Position,
		UV,
		Material,
		Bone,
		Group,
	};

	struct PMXModel::MaterialFactor
	{
		explicit MaterialFactor(
			const Eigen::Vector3f& diffuse = Eigen::Vector3f::Ones(),
			const float alpha = 1,
			const Eigen::Vector3f& specular = Eigen::Vector3f::Ones(),
			const float specularPower = 1,
			const Eigen::Vector3f& ambient = Eigen::Vector3f::Ones(),
			const Eigen::Vector4f& edgeColor = Eigen::Vector4f::Ones(),
			const float edgeSize = 1,
			const Eigen::Vector4f& textureFactor = Eigen::Vector4f::Ones(),
			const Eigen::Vector4f& spTextureFactor = Eigen::Vector4f::Ones(),
			const Eigen::Vector4f& toonTextureFactor = Eigen::Vector4f::Ones()
			): m_diffuse(diffuse),
			m_alpha(alpha),
			m_specular(specular),
			m_specularPower(specularPower),
			m_ambient(ambient),
			m_edgeColor(edgeColor),
			m_edgeSize(edgeSize),
			m_textureFactor(textureFactor),
			m_spTextureFactor(spTextureFactor),
			m_toonTextureFactor(toonTextureFactor)
		{}

		explicit MaterialFactor(const PMXFileMorph::MaterialMorph& pmxMat):
			m_diffuse(pmxMat.m_diffuse.head<3>()),
			m_alpha(pmxMat.m_diffuse.w()),
			m_specular(pmxMat.m_specular),
			m_specularPower(pmxMat.m_specularPower),
			m_ambient(pmxMat.m_ambient),
			m_edgeColor(pmxMat.m_edgeColor),
			m_edgeSize(pmxMat.m_edgeSize),
			m_textureFactor(pmxMat.m_textureFactor),
			m_spTextureFactor(pmxMat.m_sphereTextureFactor),
			m_toonTextureFactor(pmxMat.m_toonTextureFactor)
		{}

		void Mul(const MaterialFactor& val, float weight)
		{
			m_diffuse = m_diffuse + (m_diffuse.cwiseProduct(val.m_diffuse) - m_diffuse) * weight;
			m_alpha = m_alpha + (m_alpha * val.m_alpha - m_alpha) * weight;
			m_specular = m_specular + (m_specular.cwiseProduct(val.m_specular) - m_specular) * weight;
			m_specularPower = m_specularPower + (m_specularPower * val.m_specularPower - m_specularPower) * weight;
			m_ambient = m_ambient + (m_ambient.cwiseProduct(val.m_ambient) - m_ambient) * weight;
			m_edgeColor = m_edgeColor + (m_edgeColor.cwiseProduct(val.m_edgeColor) - m_edgeColor) * weight;
			m_edgeSize = m_edgeSize + (m_edgeSize * val.m_edgeSize - m_edgeSize) * weight;
			m_textureFactor = m_textureFactor + (m_textureFactor.cwiseProduct(val.m_textureFactor) - m_textureFactor) * weight;
			m_spTextureFactor = m_spTextureFactor + (m_spTextureFactor.cwiseProduct(val.m_spTextureFactor) - m_spTextureFactor) * weight;
			m_toonTextureFactor = m_toonTextureFactor + (m_toonTextureFactor.cwiseProduct(val.m_toonTextureFactor) - m_toonTextureFactor) * weight;
		}

		void Add(const MaterialFactor& val, float weight)
		{
			m_diffuse += val.m_diffuse * weight;
			m_alpha += val.m_alpha * weight;
			m_specular += val.m_specular * weight;
			m_specularPower += val.m_specularPower * weight;
			m_ambient += val.m_ambient * weight;
			m_edgeColor += val.m_edgeColor * weight;
			m_edgeSize += val.m_edgeSize * weight;
			m_textureFactor += val.m_textureFactor * weight;
			m_spTextureFactor += val.m_spTextureFactor * weight;
			m_toonTextureFactor += val.m_toonTextureFactor * weight;
		}

		Eigen::Vector3f	m_diffuse;
		float		m_alpha;
		Eigen::Vector3f	m_specular;
		float		m_specularPower;
		Eigen::Vector3f	m_ambient;
		Eigen::Vector4f	m_edgeColor;
		float		m_edgeSize;
		Eigen::Vector4f	m_textureFactor;
		Eigen::Vector4f	m_spTextureFactor;
		Eigen::Vector4f	m_toonTextureFactor;
	};

	struct PMXModel::MaterialMorphData
	{
		std::vector<PMXFileMorph::MaterialMorph>	m_materialMorphs;

		MaterialMorphData() = default;
		~MaterialMorphData() = default;
		MaterialMorphData(const MaterialMorphData&) = delete;
		MaterialMorphData& operator=(const MaterialMorphData&) = delete;
		MaterialMorphData(MaterialMorphData&& other) noexcept
			: m_materialMorphs(std::move(other.m_materialMorphs)){}
		MaterialMorphData& operator=(MaterialMorphData&& other) noexcept
		{
			if (this != &other)
			{
				m_materialMorphs = std::move(other.m_materialMorphs);
			}
			return *this;
		}
	};

	struct PMXModel::BoneMorphElement
	{
		explicit BoneMorphElement(MMDNode* node = nullptr, const Eigen::Vector3f& position = Eigen::Vector3f::Zero(), const Eigen::Quaternionf& rotate = Eigen::Quaternionf::Identity())
			: m_node(node), m_position(position), m_rotate(rotate) {}

		MMDNode*	m_node;
		Eigen::Vector3f	m_position;
		Eigen::Quaternionf	m_rotate;
	};

	struct PMXModel::BoneMorphData
	{
		std::vector<BoneMorphElement>	m_boneMorphs;

		BoneMorphData() = default;
		~BoneMorphData() = default;
		BoneMorphData(const BoneMorphData&) = delete;
		BoneMorphData& operator=(const BoneMorphData&) = delete;
		BoneMorphData(BoneMorphData&& other) noexcept : m_boneMorphs(std::move(other.m_boneMorphs)){}
		BoneMorphData& operator=(BoneMorphData&& other) noexcept
		{
			if (this != &other)
			{
				m_boneMorphs = std::move(other.m_boneMorphs);
			}
			return *this;
		}
	};

	struct PMXModel::GroupMorphData
	{
		std::vector<PMXFileMorph::GroupMorph>		m_groupMorphs;

		GroupMorphData() = default;
		~GroupMorphData() = default;
		GroupMorphData(const GroupMorphData&) = delete;
		GroupMorphData& operator=(const GroupMorphData&) = delete;
		GroupMorphData(GroupMorphData&& other) noexcept: m_groupMorphs(std::move(other.m_groupMorphs)){}
		GroupMorphData& operator=(GroupMorphData&& other) noexcept
		{
			if (this != &other)
			{
				m_groupMorphs = std::move(other.m_groupMorphs);
			}
			return *this;
		}
	};

	PMXModel::PMXModel() = default;

	PMXModel::~PMXModel()
	{
		m_materials.clear();
		m_subMeshes.clear();
		m_nodeMan.GetNodes()->clear();
	}

	void PMXModel::InitializeAnimation()
	{
		ClearBaseAnimation();

		for (const auto& node : *m_nodeMan.GetNodes())
		{
			node->SetAnimationTranslate(Eigen::Vector3f::Zero());
			node->SetAnimationRotate(Eigen::Quaternionf::Identity());
		}

		BeginAnimation();

		for (const auto& node : *m_nodeMan.GetNodes())
		{
			node->UpdateLocalTransform();
		}

		for (const auto& morph : *m_morphMan.GetMorphs())
		{
			morph->SetWeight(0);
		}

		for (const auto& ikSolver : *m_ikSolverMan.GetIKSolvers())
		{
			ikSolver->Enable(true);
		}

		for (const auto& node : *m_nodeMan.GetNodes())
		{
			if (node->GetParent() == nullptr)
			{
				node->UpdateGlobalTransform();
			}
		}

		for (const auto pmxNode : m_sortedNodes)
		{
			if (pmxNode->GetAppendNode() != nullptr)
			{
				pmxNode->UpdateAppendTransform();
				pmxNode->UpdateGlobalTransform();
			}
			if (pmxNode->GetIKSolver() != nullptr)
			{
				pmxNode->GetIKSolver()->Solve();
			}
		}

		EndAnimation();

		ResetPhysics();

		const auto rigidbodys = m_physicsMan.GetRigidBodys();
		for (const auto& rb : *rigidbodys)
		{
			rb->SetActivation(true);
		}
	}

	void PMXModel::BeginAnimation()
	{
		for (const auto& node : *m_nodeMan.GetNodes())
		{
			node->BeginUpdateTransform();
		}
	}

	void PMXModel::EndAnimation()
	{
		for (const auto& node : *m_nodeMan.GetNodes())
		{
			node->EndUpdateTransform();
		}
	}

	void PMXModel::UpdateNodeAnimation(const bool afterPhysicsAnim)
	{
		const auto& nodes = afterPhysicsAnim ? m_afterPhysicsNodes : m_beforePhysicsNodes;

		for (const auto pmxNode : nodes)
		{
			pmxNode->UpdateLocalTransform();
		}

		for (const auto pmxNode : nodes)
		{
			if (pmxNode->GetParent() == nullptr)
			{
				pmxNode->UpdateGlobalTransform();
			}
		}

		// Nodes are sorted by deform depth, so parent transforms are always
		// resolved before children. Each Append/IK node propagates via DFS to
		// its subtree, making a final full-tree DFS unnecessary.
		for (const auto pmxNode : nodes)
		{
			if (pmxNode->GetAppendNode() != nullptr)
			{
				pmxNode->UpdateAppendTransform();
				pmxNode->UpdateGlobalTransform();
			}
			if (pmxNode->GetIKSolver() != nullptr)
			{
				pmxNode->GetIKSolver()->Solve();
			}
		}
	}

	void PMXModel::ResetPhysics()
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

	void PMXModel::UpdatePhysicsAnimation(const float elapsed)
	{
		MMDPhysicsManager* physicsMan = GetPhysicsManager();
		const auto physics = physicsMan->GetMMDPhysics();

		if (physics == nullptr)
		{
			return;
		}

		physics->Update(elapsed);

		const auto rigidbodys = physicsMan->GetRigidBodys();
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

	void PMXModel::Destroy()
	{
		m_materials.clear();
		m_subMeshes.clear();
		m_nodeMan.GetNodes()->clear();
	}

	void PMXModel::SortNodes()
	{
		m_sortedNodes.clear();
		m_sortedNodes.reserve(m_nodeMan.GetNodeCount());
		auto* pmxNodes = m_nodeMan.GetNodes();
		for (auto& pmxNode : (*pmxNodes))
		{
			m_sortedNodes.push_back(pmxNode.get());
		}
		std::stable_sort(
			m_sortedNodes.begin(),
			m_sortedNodes.end(),
			[](const PMXNode* x, const PMXNode* y) { return x->GetDeformDepth() < y->GetDeformDepth(); }
		);

		m_beforePhysicsNodes.clear();
		m_afterPhysicsNodes.clear();
		for (auto* node : m_sortedNodes)
		{
			if (node->IsDeformAfterPhysics())
				m_afterPhysicsNodes.push_back(node);
			else
				m_beforePhysicsNodes.push_back(node);
		}
	}

	PMXModel::PMXMorph::PMXMorph(): m_morphType(MorphType::None), m_dataIndex()
	{}

	bool PMXModel::Load(const std::string& filepath, const std::string& mmdDataDir)
	{
		Destroy();

		PMXFile pmx;
		if (!ReadPMXFile(&pmx, filepath.c_str()))
		{
			return false;
		}

		if (!LoadPMX(pmx, PathUtil::GetDirectoryName(filepath), mmdDataDir))
		{
			return false;
		}

		return true;
	}

	bool PMXModel::LoadPMX(const PMXFile& file, const std::string& dirPath, const std::string& mmdDataDir)
	{
		m_modelName = file.m_info.m_modelName;
		m_englishModelName = file.m_info.m_englishModelName;
		m_comment = file.m_info.m_comment;
		m_englishComment = file.m_info.m_englishComment;

		std::vector<std::string> texturePaths;
		texturePaths.reserve(file.m_textures.size());
		for (const auto& [m_textureName] : file.m_textures)
		{
			std::string texPath = PathUtil::Combine(dirPath, m_textureName);
			texturePaths.emplace_back(std::move(texPath));
		}

		// Load materials
		m_materials.reserve(file.m_materials.size());
		m_subMeshes.reserve(file.m_materials.size());
		uint32_t beginIndex = 0;
		for (const auto& pmxMat : file.m_materials)
		{
			MMDMaterial mat;
			mat.m_diffuse = pmxMat.m_diffuse.head<3>();
			mat.m_alpha = pmxMat.m_diffuse.w();
			mat.m_specularPower = pmxMat.m_specularPower;
			mat.m_specular = pmxMat.m_specular;
			mat.m_ambient = pmxMat.m_ambient;
			mat.m_spTextureMode = MMDMaterial::SphereTextureMode::None;
			mat.m_bothFace = !!(static_cast<uint8_t>(pmxMat.m_drawMode) & static_cast<uint8_t>(PMXDrawModeFlags::BothFace));
			mat.m_edgeFlag = (static_cast<uint8_t>(pmxMat.m_drawMode) & static_cast<uint8_t>(PMXDrawModeFlags::DrawEdge)) == 0 ? 0 : 1;
			mat.m_groundShadow = !!(static_cast<uint8_t>(pmxMat.m_drawMode) & static_cast<uint8_t>(PMXDrawModeFlags::GroundShadow));
			mat.m_shadowCaster = !!(static_cast<uint8_t>(pmxMat.m_drawMode) & static_cast<uint8_t>(PMXDrawModeFlags::CastSelfShadow));
			mat.m_shadowReceiver = !!(static_cast<uint8_t>(pmxMat.m_drawMode) & static_cast<uint8_t>(PMXDrawModeFlags::RecieveSelfShadow));
			mat.m_edgeSize = pmxMat.m_edgeSize;
			mat.m_edgeColor = pmxMat.m_edgeColor;

			// Texture
			if (pmxMat.m_textureIndex != -1)
			{
				mat.m_texture = PathUtil::Normalize(texturePaths[pmxMat.m_textureIndex]);
			}

			// ToonTexture
			if (pmxMat.m_toonMode == PMXToonMode::Common)
			{
				if (pmxMat.m_toonTextureIndex != -1)
				{
					std::stringstream ss;
					ss << "toon" << std::setfill('0') << std::setw(2) << (pmxMat.m_toonTextureIndex + 1) << ".bmp";
					mat.m_toonTexture = PathUtil::Combine(mmdDataDir, ss.str());
				}
			}
			else if (pmxMat.m_toonMode == PMXToonMode::Separate)
			{
				if (pmxMat.m_toonTextureIndex != -1)
				{
					mat.m_toonTexture = PathUtil::Normalize(texturePaths[pmxMat.m_toonTextureIndex]);
				}
			}

			// SpTexture
			if (pmxMat.m_sphereTextureIndex != -1)
			{
				mat.m_spTexture = PathUtil::Normalize(texturePaths[pmxMat.m_sphereTextureIndex]);
				mat.m_spTextureMode = MMDMaterial::SphereTextureMode::None;
				if (pmxMat.m_sphereMode == PMXSphereMode::Mul)
				{
					mat.m_spTextureMode = MMDMaterial::SphereTextureMode::Mul;
				}
				else if (pmxMat.m_sphereMode == PMXSphereMode::Add)
				{
					mat.m_spTextureMode = MMDMaterial::SphereTextureMode::Add;
				}
				else if (pmxMat.m_sphereMode == PMXSphereMode::SubTexture)
				{
					// TODO: SphareTexture is SubTexture
				}
			}

			m_materials.emplace_back(std::move(mat));

			MMDSubMesh subMesh;
			subMesh.m_beginIndex = static_cast<int>(beginIndex);
			subMesh.m_vertexCount = pmxMat.m_numFaceVertices;
			subMesh.m_materialID = static_cast<int>(m_materials.size() - 1);
			m_subMeshes.push_back(subMesh);

			beginIndex = beginIndex + pmxMat.m_numFaceVertices;
		}
		m_initMaterials = m_materials;
		m_mulMaterialFactors.resize(m_materials.size());
		m_addMaterialFactors.resize(m_materials.size());

		// Create bone nodes
		m_nodeMan.GetNodes()->reserve(file.m_bones.size());
		for (const auto& bone : file.m_bones)
		{
			auto* node = m_nodeMan.AddNode();
			node->SetName(bone.m_name);
		}

		// Set bone hierarchy and transforms
		for (size_t i = 0; i < file.m_bones.size(); i++)
		{
			auto boneIndex = static_cast<int32_t>(file.m_bones.size() - i - 1);
			const auto& bone = file.m_bones[boneIndex];
			auto* node = m_nodeMan.GetNode(boneIndex);

			// Check if the node is looping
			bool isLooping = false;
			if (bone.m_parentBoneIndex != -1)
			{
				MMDNode* parent = m_nodeMan.GetNode(bone.m_parentBoneIndex);
				while (parent != nullptr)
				{
					if (parent == node)
					{
						isLooping = true;
						LIBMMD_ERROR("This bone hierarchy is a loop: bone={}", boneIndex);
						break;
					}
					parent = parent->GetParent();
				}
			}

			// Check parent node index
			if (bone.m_parentBoneIndex != -1)
			{
				if (bone.m_parentBoneIndex >= boneIndex)
				{
					LIBMMD_WARN("The parent index of this node is big: bone={}", boneIndex);
				}
			}

			if ((bone.m_parentBoneIndex != -1) && !isLooping)
			{
				const auto& parentBone = file.m_bones[bone.m_parentBoneIndex];
				auto* parent = m_nodeMan.GetNode(bone.m_parentBoneIndex);
				parent->AddChild(node);
				Eigen::Vector3f localPos = bone.m_position - parentBone.m_position;
				node->SetTranslate(localPos);
			}
			else
			{
				auto localPos = bone.m_position;
				node->SetTranslate(localPos);
			}
			Eigen::Matrix4f init = Eigen::Matrix4f::Identity();
			init.block<3,1>(0,3) = bone.m_position;
			node->SetGlobalTransform(init);
			node->CalculateInverseInitTransform();

			node->SetDeformDepth(bone.m_deformDepth);
			bool deformAfterPhysics = !!(static_cast<uint16_t>(bone.m_boneFlag) & static_cast<uint16_t>(PMXBoneFlags::DeformAfterPhysics));
			node->EnableDeformAfterPhysics(deformAfterPhysics);
			bool appendRotate = (static_cast<uint16_t>(bone.m_boneFlag) & static_cast<uint16_t>(PMXBoneFlags::AppendRotate)) != 0;
			bool appendTranslate = (static_cast<uint16_t>(bone.m_boneFlag) & static_cast<uint16_t>(PMXBoneFlags::AppendTranslate)) != 0;
			node->EnableAppendRotate(appendRotate);
			node->EnableAppendTranslate(appendTranslate);
			if ((appendRotate || appendTranslate) && (bone.m_appendBoneIndex != -1))
			{
				if (bone.m_appendBoneIndex >= boneIndex)
				{
					LIBMMD_WARN("The parent(morph assignment) index of this node is big: bone={}", boneIndex);
				}
				bool appendLocal = (static_cast<uint16_t>(bone.m_boneFlag) & static_cast<uint16_t>(PMXBoneFlags::AppendLocal)) != 0;
				auto appendNode = m_nodeMan.GetNode(bone.m_appendBoneIndex);
				float appendWeight = bone.m_appendWeight;
				node->EnableAppendLocal(appendLocal);
				node->SetAppendNode(appendNode);
				node->SetAppendWeight(appendWeight);
			}
			node->SaveInitialTRS();
		}

		SortNodes();

		// IK
		for (size_t i = 0; i < file.m_bones.size(); i++)
		{
			const auto& bone = file.m_bones[i];
			if (static_cast<uint16_t>(bone.m_boneFlag) & static_cast<uint16_t>(PMXBoneFlags::IK))
			{
				auto solver = m_ikSolverMan.AddIKSolver();
				auto* ikNode = m_nodeMan.GetNode(i);
				solver->SetIKNode(ikNode);
				ikNode->SetIKSolver(solver);

				if ((bone.m_ikTargetBoneIndex < 0) || (bone.m_ikTargetBoneIndex >= static_cast<int>(m_nodeMan.GetNodeCount())))
				{
					LIBMMD_ERROR("Wrong IK Target: bone={} target={}", i, bone.m_ikTargetBoneIndex);
					continue;
				}

				auto* targetNode = m_nodeMan.GetNode(bone.m_ikTargetBoneIndex);
				solver->SetTargetNode(targetNode);

				for (const auto& ikLink : bone.m_ikLinks)
				{
					auto* linkNode = m_nodeMan.GetNode(ikLink.m_ikBoneIndex);
					if (ikLink.m_enableLimit)
					{
					solver->AddIKChain(linkNode, true, ikLink.m_limitMin, ikLink.m_limitMax);
					}
					else
					{
						solver->AddIKChain(linkNode);
					}
					linkNode->EnableIK(true);
				}

				solver->SetIterateCount(bone.m_ikIterationCount);
				solver->SetLimitAngle(bone.m_ikLimit);
				solver->BuildChainPath();
			}
		}

		LoadMorph(file);

		// Physics
		if (!m_physicsMan.Create())
		{
			LIBMMD_ERROR("Create Physics Fail.");
			return false;
		}

		for (const auto& pmxRB : file.m_rigidbodies)
		{
			auto rb = m_physicsMan.AddRigidBody();
			MMDNode* node = nullptr;
			if (pmxRB.m_boneIndex != -1)
			{
				node = m_nodeMan.GetMMDNode(pmxRB.m_boneIndex);
			}
			if (!rb->Create(pmxRB, this, node))
			{
				LIBMMD_ERROR("Create Rigid Body Fail.\n");
				return false;
			}
			m_physicsMan.GetMMDPhysics()->AddRigidBody(rb);
		}

		for (const auto& pmxJoint : file.m_joints)
		{
			if (pmxJoint.m_rigidbodyAIndex != -1 &&
				pmxJoint.m_rigidbodyBIndex != -1 &&
				pmxJoint.m_rigidbodyAIndex != pmxJoint.m_rigidbodyBIndex)
			{
				auto joint = m_physicsMan.AddJoint();
				auto rigidBodys = m_physicsMan.GetRigidBodys();
				bool ret = joint->CreateJoint(
					pmxJoint,
					(*rigidBodys)[pmxJoint.m_rigidbodyAIndex].get(),
					(*rigidBodys)[pmxJoint.m_rigidbodyBIndex].get()
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
				LIBMMD_WARN("Illegal Joint [{}]", pmxJoint.m_name.c_str());
			}
		}

		ResetPhysics();

		return true;
	}

	void PMXModel::Morph(const PMXMorph* morph, float weight)
	{
		std::stack<std::pair<const PMXMorph*, float>> morphStack;
		morphStack.emplace(morph, weight);

		while (!morphStack.empty())
		{
			auto [currentMorph, currentWeight] = morphStack.top();
			morphStack.pop();

			switch (currentMorph->m_morphType)
			{
			case MorphType::Material:
				MorphMaterial(m_materialMorphDatas[currentMorph->m_dataIndex], currentWeight);
				break;
			case MorphType::Bone:
				MorphBone(m_boneMorphDatas[currentMorph->m_dataIndex], currentWeight);
				break;
			case MorphType::Group:
				{
					const auto& [m_groupMorphs] = m_groupMorphDatas[currentMorph->m_dataIndex];
					for (const auto& [m_morphIndex, m_weight] : m_groupMorphs)
					{
						if (m_morphIndex == -1)
						{
							continue;
						}
						auto& elemMorph = (*m_morphMan.GetMorphs())[m_morphIndex];
						morphStack.emplace(elemMorph.get(), m_weight * currentWeight);
					}
					break;
				}
			default:
				break;
			}
		}
	}

	void PMXModel::LoadMorph(const PMXFile& file)
	{
		// Process morphs
		for (const auto& morph : file.m_morphs)
		{
			auto* pmxMorph = static_cast<PMXMorph*>(m_morphMan.AddMorph());
			pmxMorph->SetName(morph.m_name);
			pmxMorph->SetWeight(0);
			pmxMorph->m_dataIndex = -1;

			switch (morph.m_morphType)
			{
			case PMXMorphType::Position:
				{
					pmxMorph->m_morphType = MorphType::Position;
				}
				break;
			case PMXMorphType::UV:
			case PMXMorphType::AddUV1:
			case PMXMorphType::AddUV2:
			case PMXMorphType::AddUV3:
			case PMXMorphType::AddUV4:
				{
					pmxMorph->m_morphType = MorphType::UV;
				}
				break;
			case PMXMorphType::Material:
				{
					pmxMorph->m_morphType = MorphType::Material;
					auto& morphData = m_materialMorphDatas.emplace_back();
					morphData.m_materialMorphs = morph.m_materialMorph;
					pmxMorph->m_dataIndex = m_materialMorphDatas.size() - 1;
				}
				break;
			case PMXMorphType::Bone:
				{
					pmxMorph->m_morphType = MorphType::Bone;
					auto& morphData = m_boneMorphDatas.emplace_back();
					morphData.m_boneMorphs.reserve(morph.m_boneMorph.size());
					for (const auto& bone : morph.m_boneMorph)
					{
						if (bone.m_boneIndex != -1)
						{
							auto* node = m_nodeMan.GetNode(bone.m_boneIndex);
							morphData.m_boneMorphs.emplace_back(node, bone.m_position, bone.m_quaternion);
						}
					}
					pmxMorph->m_dataIndex = m_boneMorphDatas.size() - 1;
				}
				break;
			case PMXMorphType::Group:
				{
					pmxMorph->m_morphType = MorphType::Group;
					auto& morphData = m_groupMorphDatas.emplace_back();
					morphData.m_groupMorphs = morph.m_groupMorph;
					pmxMorph->m_dataIndex = m_groupMorphDatas.size() - 1;
				}
				break;
			default:
				LIBMMD_ERROR("PMX Load Error: Unknown morph type");
				break;
			}
		}

		// Check whether Group Morph infinite loop.
		{
			std::vector<int32_t> groupMorphStack;
			std::function<void(int32_t)> fixInifinitGropuMorph;
			fixInifinitGropuMorph = [this, &fixInifinitGropuMorph, &groupMorphStack](int32_t morphIdx)
			{
				const auto& morphs = (*m_morphMan.GetMorphs());
				if (const auto& morph = morphs[morphIdx]; morph->m_morphType == MorphType::Group)
				{
					auto& groupMorphData = m_groupMorphDatas[morph->m_dataIndex];
					for (size_t i = 0; i < groupMorphData.m_groupMorphs.size(); i++)
					{
						auto& groupMorph = groupMorphData.m_groupMorphs[i];

						auto findIt = std::find(
							groupMorphStack.begin(),
							groupMorphStack.end(),
							groupMorph.m_morphIndex
						);
						if (findIt != groupMorphStack.end())
						{
							LIBMMD_WARN("Infinit Group Morph:[{}][{}][{}]",
								morphIdx, morph->GetName(), i
							);
							groupMorph.m_morphIndex = -1;
						}
						else
						{
							groupMorphStack.push_back(morphIdx);
							if (groupMorph.m_morphIndex>0)
								fixInifinitGropuMorph(groupMorph.m_morphIndex);
							else
								LIBMMD_ERROR("Invalid morph index: group={}, morph={}", groupMorph.m_morphIndex, morphIdx);
							groupMorphStack.pop_back();
						}
					}
				}
			};

			for (int32_t morphIdx = 0; morphIdx < static_cast<int32_t>(m_morphMan.GetMorphCount()); morphIdx++)
			{
				fixInifinitGropuMorph(morphIdx);
				groupMorphStack.clear();
			}
		}
	}

	void PMXModel::BeginMorphMaterial()
	{
		MaterialFactor initMul{
			Eigen::Vector3f::Ones(), 1, Eigen::Vector3f::Ones(), 1, Eigen::Vector3f::Ones(),
			Eigen::Vector4f::Ones(), 1, Eigen::Vector4f::Ones(), Eigen::Vector4f::Ones(), Eigen::Vector4f::Ones()
		};

		MaterialFactor initAdd{
			Eigen::Vector3f::Zero(), 0, Eigen::Vector3f::Zero(), 0, Eigen::Vector3f::Zero(),
			Eigen::Vector4f::Zero(), 0, Eigen::Vector4f::Zero(), Eigen::Vector4f::Zero(), Eigen::Vector4f::Zero()
		};

		const size_t matCount = m_materials.size();
		for (size_t matIdx = 0; matIdx < matCount; matIdx++)
		{
			m_mulMaterialFactors[matIdx] = initMul;
			m_mulMaterialFactors[matIdx].m_diffuse = m_materials[matIdx].m_diffuse;
			m_mulMaterialFactors[matIdx].m_alpha = m_materials[matIdx].m_alpha;
			m_mulMaterialFactors[matIdx].m_specular = m_materials[matIdx].m_specular;
			m_mulMaterialFactors[matIdx].m_specularPower = m_materials[matIdx].m_specularPower;
			m_mulMaterialFactors[matIdx].m_ambient = m_materials[matIdx].m_ambient;

			m_addMaterialFactors[matIdx] = initAdd;
		}
	}

	void PMXModel::EndMorphMaterial()
	{
		const size_t matCount = m_materials.size();
		for (size_t matIdx = 0; matIdx < matCount; matIdx++)
		{
			MaterialFactor matFactor = m_mulMaterialFactors[matIdx];
			matFactor.Add(m_addMaterialFactors[matIdx], 1.0f);

			m_materials[matIdx].m_diffuse = matFactor.m_diffuse;
			m_materials[matIdx].m_alpha = matFactor.m_alpha;
			m_materials[matIdx].m_specular = matFactor.m_specular;
			m_materials[matIdx].m_specularPower = matFactor.m_specularPower;
			m_materials[matIdx].m_ambient = matFactor.m_ambient;
			m_materials[matIdx].m_textureMulFactor = m_mulMaterialFactors[matIdx].m_textureFactor;
			m_materials[matIdx].m_textureAddFactor = m_addMaterialFactors[matIdx].m_textureFactor;
			m_materials[matIdx].m_spTextureMulFactor = m_mulMaterialFactors[matIdx].m_spTextureFactor;
			m_materials[matIdx].m_spTextureAddFactor = m_addMaterialFactors[matIdx].m_spTextureFactor;
			m_materials[matIdx].m_toonTextureMulFactor = m_mulMaterialFactors[matIdx].m_toonTextureFactor;
			m_materials[matIdx].m_toonTextureAddFactor = m_addMaterialFactors[matIdx].m_toonTextureFactor;
		}
	}

	void PMXModel::MorphMaterial(const MaterialMorphData & morphData, const float weight)
	{
		for (const auto& matMorph : morphData.m_materialMorphs)
		{
			if (matMorph.m_materialIndex != -1)
			{
				const auto mi = matMorph.m_materialIndex;
				switch (matMorph.m_opType)
				{
				case libmmd::PMXFileMorph::MaterialMorph::OpType::Mul:
					m_mulMaterialFactors[mi].Mul(
						MaterialFactor(matMorph),
						weight
					);
					break;
				case libmmd::PMXFileMorph::MaterialMorph::OpType::Add:
					m_addMaterialFactors[mi].Add(
						MaterialFactor(matMorph),
						weight
					);
					break;
				default:
					break;
				}
			}
			else
			{
				switch (matMorph.m_opType)
				{
				case libmmd::PMXFileMorph::MaterialMorph::OpType::Mul:
					for (size_t i = 0; i < m_materials.size(); i++)
					{
						m_mulMaterialFactors[i].Mul(
							MaterialFactor(matMorph),
							weight
						);
					}
					break;
				case libmmd::PMXFileMorph::MaterialMorph::OpType::Add:
					for (size_t i = 0; i < m_materials.size(); i++)
					{
						m_addMaterialFactors[i].Add(
							MaterialFactor(matMorph),
							weight
						);
					}
					break;
				default:
					break;
				}
			}
		}
	}

	void PMXModel::MorphBone(const BoneMorphData & morphData, const float weight)
	{
		for (const auto& [m_node, m_position, m_rotate] : morphData.m_boneMorphs)
		{
			const auto node = m_node;
			Eigen::Vector3f t = m_position * weight;
			node->SetTranslate(node->GetTranslate() + t);
			Eigen::Quaternionf q = node->GetRotate().slerp(weight, m_rotate);
			node->SetRotate(q);
		}
	}

	void PMXModel::UpdateMorphAnimation()
	{
		// Process morphs
		BeginMorphMaterial();

		const auto& morphs = *m_morphMan.GetMorphs();
		for (const auto & morph : morphs)
		{
			if (morph->GetWeight() != 0.0f)
				Morph(morph.get(), morph->GetWeight());
		}

		EndMorphMaterial();
	}

	PMXNode::PMXNode()
		: m_deformDepth(-1)
		, m_isDeformAfterPhysics(false)
		, m_appendNode(nullptr)
		, m_isAppendRotate(false)
		, m_isAppendTranslate(false)
		, m_isAppendLocal(false)
		, m_appendWeight(0)
		, m_ikSolver(nullptr)
	{
	}

	void PMXNode::UpdateAppendTransform()
	{
		if (m_appendNode == nullptr)
		{
			return;
		}

		if (m_isAppendRotate)
		{
			Eigen::Quaternionf appendRotate;
			if (m_isAppendLocal)
			{
				appendRotate = m_appendNode->AnimateRotate();
			}
			else
			{
				if (m_appendNode->GetAppendNode() != nullptr)
				{
					appendRotate = m_appendNode->GetAppendRotate();
				}
				else
				{
					appendRotate = m_appendNode->AnimateRotate();
				}
			}

			if (m_appendNode->m_enableIK)
			{
				appendRotate = m_appendNode->GetIKRotate() * appendRotate;
			}

			const Eigen::Quaternionf appendQ = Eigen::Quaternionf::Identity().slerp(GetAppendWeight(), appendRotate);
			m_appendRotate = appendQ;
		}

		if (m_isAppendTranslate)
		{
			Eigen::Vector3f appendTranslate = Eigen::Vector3f::Zero();
			if (m_isAppendLocal)
			{
				appendTranslate = m_appendNode->GetTranslate() - m_appendNode->GetInitialTranslate();
			}
			else
			{
				if (m_appendNode->GetAppendNode() != nullptr)
				{
					appendTranslate = m_appendNode->GetAppendTranslate();
				}
				else
				{
					appendTranslate = m_appendNode->GetTranslate() - m_appendNode->GetInitialTranslate();
				}
			}

			m_appendTranslate = appendTranslate * GetAppendWeight();
		}

		UpdateLocalTransform();
	}

	void PMXNode::OnBeginUpdateTransform()
	{
		m_appendTranslate = Eigen::Vector3f::Zero();
		m_appendRotate = Eigen::Quaternionf::Identity();
	}

	void PMXNode::OnEndUpdateTransfrom()
	{
	}

	void PMXNode::OnUpdateLocalTransform()
	{
		Eigen::Vector3f t = AnimateTranslate();
		if (m_isAppendTranslate)
		{
			t += m_appendTranslate;
		}

		Eigen::Quaternionf r = AnimateRotate();
		if (m_enableIK)
		{
			r = GetIKRotate() * r;
		}
		if (m_isAppendRotate)
		{
			r = r * m_appendRotate;
		}

		const Eigen::Vector3f s = GetScale();

		Eigen::Matrix4f tMat = Eigen::Matrix4f::Identity();
		tMat.block<3,1>(0,3) = t;
		Eigen::Matrix4f rMat = Eigen::Matrix4f::Identity();
		rMat.block<3,3>(0,0) = r.toRotationMatrix();
		Eigen::Matrix4f sMat = Eigen::Matrix4f::Identity();
		sMat.diagonal().head<3>() = s;
		m_local = tMat * rMat * sMat;
	}
}
