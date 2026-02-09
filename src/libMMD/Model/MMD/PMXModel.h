//
// Copyright(c) 2016-2017 benikabocha.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)
//

#ifndef LIBMMD_MODEL_MMD_PMXMODEL_H_
#define LIBMMD_MODEL_MMD_PMXMODEL_H_

#include "MMDMaterial.h"
#include "MMDModel.h"
#include "MMDIkSolver.h"

#include <glm/gtc/quaternion.hpp>
#include <vector>
#include <string>

namespace libmmd
{
	struct PMXFile;
	/**
	 * @brief PMXNode class represents a node in the PMX model.
	 */
	class PMXNode final : public MMDNode
	{
	public:
		PMXNode();
		~PMXNode() override = default;

		/**
		 * @brief Set the deform depth.
		 * @param depth Deform depth.
		 */
		void SetDeformDepth(const int32_t depth) { m_deformDepth = depth; }

		/**
		 * @brief Get the deform depth.
		 * @return Deform depth.
		 */
		int32_t GetDeformDepth() const { return m_deformDepth; }

		/**
		 * @brief Enable or disable deform after physics.
		 * @param enable True to enable, false to disable.
		 */
		void EnableDeformAfterPhysics(const bool enable) { m_isDeformAfterPhysics = enable; }

		/**
		 * @brief Check if deform after physics is enabled.
		 * @return True if enabled, false otherwise.
		 */
		bool IsDeformAfterPhysics() const { return m_isDeformAfterPhysics; }

		/**
		 * @brief Set the append node.
		 * @param node Pointer to the append node.
		 */
		void SetAppendNode(PMXNode* node) { m_appendNode = node; }

		/**
		 * @brief Get the append node.
		 * @return Pointer to the append node.
		 */
		PMXNode* GetAppendNode() const { return m_appendNode; }

		/**
		 * @brief Enable or disable append rotation.
		 * @param enable True to enable, false to disable.
		 */
		void EnableAppendRotate(const bool enable) { m_isAppendRotate = enable; }

		/**
		 * @brief Enable or disable append translation.
		 * @param enable True to enable, false to disable.
		 */
		void EnableAppendTranslate(const bool enable) { m_isAppendTranslate = enable; }

		/**
		 * @brief Enable or disable append local.
		 * @param enable True to enable, false to disable.
		 */
		void EnableAppendLocal(const bool enable) { m_isAppendLocal = enable; }

		/**
		 * @brief Set the append weight.
		 * @param weight Append weight.
		 */
		void SetAppendWeight(const float weight) { m_appendWeight = weight; }

		/**
		 * @brief Get the append weight.
		 * @return Append weight.
		 */
		float GetAppendWeight() const { return m_appendWeight; }

		/**
		 * @brief Get the append translation.
		 * @return Append translation.
		 */
		const glm::vec3& GetAppendTranslate() const { return m_appendTranslate; }

		/**
		 * @brief Get the append rotation.
		 * @return Append rotation.
		 */
		const glm::quat& GetAppendRotate() const { return m_appendRotate; }

		/**
		 * @brief Set the IK solver.
		 * @param ik Pointer to the IK solver.
		 */
		void SetIKSolver(MMDIkSolver* ik) { m_ikSolver = ik; }

		/**
		 * @brief Get the IK solver.
		 * @return Pointer to the IK solver.
		 */
		MMDIkSolver* GetIKSolver() const { return m_ikSolver; }

		/**
		 * @brief Update the append transform.
		 */
		void UpdateAppendTransform();

	protected:
		void OnBeginUpdateTransform() override;
		void OnEndUpdateTransfrom() override;
		void OnUpdateLocalTransform() override;

	private:
		int32_t		m_deformDepth;
		bool		m_isDeformAfterPhysics;

		PMXNode*	m_appendNode;
		bool		m_isAppendRotate;
		bool		m_isAppendTranslate;
		bool		m_isAppendLocal;
		float		m_appendWeight;

		glm::vec3	m_appendTranslate{};
		glm::quat	m_appendRotate{};

		MMDIkSolver*	m_ikSolver;
	};

	/**
	 * @brief Represents a PMX model.
	 */
	class PMXModel : public MMDModel
	{
	public:
		PMXModel();
		~PMXModel() override;

		/**
		 * @brief Get the node manager.
		 * @return Pointer to the node manager.
		 */
		MMDNodeManager* GetNodeManager() override { return &m_nodeMan; }

		/**
		 * @brief Get the IK manager.
		 * @return Pointer to the IK manager.
		 */
		MMDIKManager* GetIKManager() override { return &m_ikSolverMan; }

		/**
		 * @brief Get the morph manager.
		 * @return Pointer to the morph manager.
		 */
		MMDMorphManager* GetMorphManager() override { return &m_morphMan; }

		/**
		 * @brief Get the physics manager.
		 * @return Pointer to the physics manager.
		 */
		MMDPhysicsManager* GetPhysicsManager() override { return &m_physicsMan; }

		/**
		 * @brief Get the material count.
		 * @return Material count.
		 */
		size_t GetMaterialCount() const override { return m_materials.size(); }

		/**
		 * @brief Get the materials.
		 * @return Pointer to the materials array.
		 */
		const MMDMaterial* GetMaterials() const override { return &m_materials[0]; }

		/**
		 * @brief Get the sub-mesh count.
		 * @return Sub-mesh count.
		 */
		size_t GetSubMeshCount() const override { return m_subMeshes.size(); }

		/**
		 * @brief Get the sub-meshes.
		 * @return Pointer to the sub-meshes array.
		 */
		const MMDSubMesh* GetSubMeshes() const override { return &m_subMeshes[0]; }

		/**
		 * @brief Get the MMD physics.
		 * @return Pointer to the MMD physics.
		 */
		MMDPhysics* GetMMDPhysics() override { return m_physicsMan.GetMMDPhysics(); }

		/**
		 * @brief Initialize the animation.
		 */
		void InitializeAnimation() override;

		/**
		 * @brief Begin the animation.
		 */
		void BeginAnimation() override;

		/**
		 * @brief End the animation.
		 */
		void EndAnimation() override;

		/**
		 * @brief Update the morph animation.
		 */
		void UpdateMorphAnimation() override;

		/**
		 * @brief Update the node animation.
		 * @param afterPhysicsAnim True if updating after physics animation.
		 */
		void UpdateNodeAnimation(bool afterPhysicsAnim) override;

		/**
		 * @brief Reset the physics.
		 */
		void ResetPhysics() override;

		/**
		 * @brief Update the physics animation.
		 * @param elapsed Elapsed time.
		 */
		void UpdatePhysicsAnimation(float elapsed) override;

		/**
		 * @brief Load the PMX model from a file.
		 * @param filepath Path to the PMX file.
		 * @param mmdDataDir Directory containing MMD data.
		 * @return True if loading is successful, false otherwise.
		 */
		bool Load(const std::string& filepath, const std::string& mmdDataDir) override;

		/**
		 * @brief Load model data from a PMX file.
		 * @param file A constant reference to the PMX file object.
		 * @param dirPath The directory path where the PMX file is located.
		 * @param mmdDataDir The directory path containing MMD data.
		 * @return Returns true if the loading is successful, otherwise false.
		 */
		virtual bool LoadPMX(const PMXFile& file, const std::string& dirPath, const std::string& mmdDataDir);

		/**
		 * @brief Destroy the PMX model.
		 */
		virtual void Destroy();
	protected:
		enum class MorphType;
		struct MaterialFactor;
		struct MaterialMorphData;
		struct BoneMorphElement;
		struct BoneMorphData;
		struct GroupMorphData;

		class PMXMorph : public MMDMorph
		{
		public:
			MorphType	m_morphType;
			size_t		m_dataIndex;

			PMXMorph();
		};

		std::vector<MMDMaterial>	m_materials;
		std::vector<MMDSubMesh>		m_subMeshes;
		std::vector<PMXNode*>		m_sortedNodes;

		MMDNodeManagerT<PMXNode>	m_nodeMan;
		MMDIKManagerT<MMDIkSolver>	m_ikSolverMan;
		MMDMorphManagerT<PMXMorph>	m_morphMan;
		MMDPhysicsManager			m_physicsMan;

		std::vector<BoneMorphData>		m_boneMorphDatas;
		std::vector<GroupMorphData>		m_groupMorphDatas;
		std::vector<MaterialMorphData>	m_materialMorphDatas;
		std::vector<MMDMaterial>	m_initMaterials;
		std::vector<MaterialFactor>	m_mulMaterialFactors;
		std::vector<MaterialFactor>	m_addMaterialFactors;

		virtual void Morph(const PMXMorph* morph, float weight);
		virtual void LoadMorph(const PMXFile& file);
		void BeginMorphMaterial();
		void EndMorphMaterial();
		void MorphMaterial(const MaterialMorphData& morphData, float weight);
		static void MorphBone(const BoneMorphData& morphData, float weight);
	};
}

#endif // !LIBMMD_MODEL_MMD_PMXMODEL_H_
