//
// Copyright(c) 2016-2017 benikabocha.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)
//

#ifndef LIBMMD_MODEL_MMD_PMDMODEL_H_
#define LIBMMD_MODEL_MMD_PMDMODEL_H_

#include "MMDMaterial.h"
#include "MMDModel.h"

#include <vector>
#include <string>

namespace libmmd
{
	struct PMDFile;
	/**
	 * @brief Represents a PMD model.
	 */
	class PMDModel : public MMDModel
	{
	public:
		PMDModel() = default;
		~PMDModel() override;

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
		 * @return Number of materials.
		 */
		size_t GetMaterialCount() const override { return m_materials.size(); }

		/**
		 * @brief Get the materials.
		 * @return Pointer to the materials array.
		 */
		const MMDMaterial* GetMaterials() const override { return &m_materials[0]; }

		/**
		 * @brief Get the sub-mesh count.
		 * @return Number of sub-meshes.
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
		 * @brief Call before animation.
		 */
		void BeginAnimation() override;

		/**
		 * @brief Call after animation.
		 */
		void EndAnimation() override;

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
		 * @brief Load the PMD model from file.
		 * @param filepath Path to the PMD file.
		 * @param mmdDataDir Directory containing MMD data.
		 * @return True if loading is successful, false otherwise.
		 */
		bool Load(const std::string& filepath, const std::string& mmdDataDir) override;

		/**
		 * @brief Load model data from a PMD file.
		 * @param file A constant reference to the PMD file object.
		 * @param dirPath The directory path where the PMD file is located.
		 * @param mmdDataDir The directory path containing MMD data.
		 * @return Returns true if the loading is successful, otherwise false.
		 */
		virtual bool LoadPMD(const PMDFile& file, const std::string& dirPath, const std::string& mmdDataDir);

		/**
		 * @brief Destroy the PMD model.
		 */
		virtual void Destroy();

	protected:
		struct MorphVertex
		{
			uint32_t	m_index;
			Eigen::Vector3f	m_position;

			explicit  MorphVertex(const uint32_t index = 0, const Eigen::Vector3f& position = Eigen::Vector3f::Zero());
		};

		/**
		 * @brief Represents a PMD morph.
		 */
		class PMDMorph : public MMDMorph
		{
		public:
			std::vector<MorphVertex>	m_vertices; ///< List of morph vertices
		};

		virtual void LoadMorph(const PMDFile& file);

		MMDNodeManagerT<MMDNode>	m_nodeMan;
		MMDIKManagerT<MMDIkSolver>	m_ikSolverMan;
		MMDMorphManagerT<PMDMorph>	m_morphMan;
		MMDPhysicsManager			m_physicsMan;

		std::vector<MMDMaterial>	m_materials;
		std::vector<MMDSubMesh>		m_subMeshes;
	};
}

#endif // !LIBMMD_MODEL_MMD_PMDMODEL_H_
