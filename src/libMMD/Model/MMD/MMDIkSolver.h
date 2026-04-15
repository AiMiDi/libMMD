//
// Copyright(c) 2016-2017 benikabocha.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)
//

#ifndef LIBMMD_MODEL_MMD_MMDIKSOLVER_H_
#define	LIBMMD_MODEL_MMD_MMDIKSOLVER_H_

#include "IMMDNode.h"

#include <vector>
#include <string>

namespace libmmd
{
	/**
	 * @brief Represents an IK solver in MMD.
	 */
	class MMDIkSolver final
	{
	public:
		MMDIkSolver();

		/**
		 * @brief Set the IK node.
		 * @param node Pointer to the IK node.
		 */
		void SetIKNode(IMMDNode* node)
		{
			m_ikNode = node;
			InvalidateChainPathCache();
		}

		/**
		 * @brief Set the target node.
		 * @param node Pointer to the target node.
		 */
		void SetTargetNode(IMMDNode* node)
		{
			m_ikTarget = node;
			InvalidateChainPathCache();
		}

		/**
		 * @brief Get the IK node.
		 * @return Pointer to the IK node.
		 */
		IMMDNode* GetIKNode() const { return m_ikNode; }

		/**
		 * @brief Get the target node.
		 * @return Pointer to the target node.
		 */
		IMMDNode* GetTargetNode() const { return m_ikTarget; }

		/**
		 * @brief Get the name of the IK node.
		 * @return Name of the IK node.
		 */
		std::string GetName() const
		{
			if (m_ikNode != nullptr)
			{
				return m_ikNode->GetName();
			}
			return "";
		}

		/**
		 * @brief Set the iterate count.
		 * @param count Iterate count.
		 */
		void SetIterateCount(const uint32_t count) { m_iterateCount = count; }

		/**
		 * @brief Set the limit angle.
		 * @param angle Limit angle.
		 */
		void SetLimitAngle(const float angle) { m_limitAngle = angle; }

		/**
		 * @brief Enable or disable the IK solver.
		 * @param enable Enable flag.
		 */
		void Enable(const bool enable) { m_enable = enable; }

		/**
		 * @brief Check if the IK solver is enabled.
		 * @return True if enabled, false otherwise.
		 */
		bool Enabled() const { return m_enable; }

		/**
		 * @brief Add an IK chain.
		 * @param node Pointer to the node.
		 * @param isKnee Whether the node is a knee.
		 */
		void AddIKChain(IMMDNode* node, bool isKnee = false);

		/**
		 * @brief Add an IK chain with axis limit.
		 * @param node Pointer to the node.
		 * @param axisLimit Whether to enable axis limit.
		 * @param limitMin Minimum limit.
		 * @param limitMax Maximum limit.
		 */
		void AddIKChain(
			IMMDNode* node,
			bool axisLimit,
			const Eigen::Vector3f& limitMin,
			const Eigen::Vector3f& limitMax
		);

		/**
		 * @brief Remove all IK chains, allowing a full rebuild.
		 */
		void ClearIKChains()
		{
			m_chains.clear();
			InvalidateChainPathCache();
		}

		/**
		 * @brief Solve the IK.
		 */
		void Solve();

		/**
		 * @brief Build tracked paths from the highest chain node to the
		 *        IK node and target for scoped global transform updates.
		 *        Must be called after all AddIKChain calls are done.
		 */
		void BuildChainPath();

		/**
		 * @brief Save the base animation state.
		 */
		void SaveBaseAnimation() { m_baseAnimEnable = m_enable; }

		/**
		 * @brief Load the base animation state.
		 */
		void LoadBaseAnimation() { m_enable = m_baseAnimEnable; }

		/**
		 * @brief Clear the base animation state.
		 */
		void ClearBaseAnimation() { m_baseAnimEnable = true; }

		/**
		 * @brief Check if the base animation is enabled.
		 * @return True if enabled, false otherwise.
		 */
		bool GetBaseAnimationEnabled() const { return m_baseAnimEnable; }

	private:
		struct IKChain
		{
			IMMDNode*	m_node; ///< Pointer to the node
			bool		m_enableAxisLimit; ///< Axis limit flag
			Eigen::Vector3f	m_limitMax; ///< Maximum limit
			Eigen::Vector3f	m_limitMin; ///< Minimum limit
			Eigen::Vector3f	m_prevAngle; ///< Previous angle
			Eigen::Quaternionf	m_saveIKRot; ///< Saved IK rotation
			float		m_planeModeAngle; ///< Plane mode angle

		IKChain(
			IMMDNode* node = nullptr,
			const bool enableAxisLimit = false,
			const Eigen::Vector3f& limitMin = Eigen::Vector3f::Zero(),
			const Eigen::Vector3f& limitMax = Eigen::Vector3f::Zero(),
			const Eigen::Quaternionf& saveIKRot = Eigen::Quaternionf::Identity(),
			const Eigen::Vector3f& prevAngle = Eigen::Vector3f::Zero(),
			const float planeModeAngle = 0.0f
		)
			: m_node(node),
			  m_enableAxisLimit(enableAxisLimit),
			  m_limitMin(limitMin),
			  m_limitMax(limitMax),
			  m_prevAngle(prevAngle),
			  m_saveIKRot(saveIKRot),
			  m_planeModeAngle(planeModeAngle)
			{
			}
		};

		void InvalidateChainPathCache()
		{
			m_updateRoot = nullptr;
			m_targetPath.clear();
			m_ikPath.clear();
			m_targetPathIndices.clear();
			m_ikPathIndices.clear();
		}

		void SolveCore(uint32_t iteration);

		enum class SolveAxis {
			X, ///< X axis
			Y, ///< Y axis
			Z, ///< Z axis
		};

		void SolvePlane(uint32_t iteration, size_t chainIdx, SolveAxis solveAxis);

		void UpdatePathGlobalTransform(const std::vector<IMMDNode*>& path, size_t fromPathIdx);
		void UpdateTrackedGlobalTransforms();
		void UpdateTrackedGlobalTransforms(size_t chainIdx);

		std::vector<IKChain>	m_chains; ///< List of IK chains
		IMMDNode*	m_ikNode; ///< Pointer to the IK node
		IMMDNode*	m_ikTarget; ///< Pointer to the target node
		uint32_t	m_iterateCount; ///< Iterate count
		float		m_limitAngle; ///< Limit angle
		bool		m_enable; ///< Enable flag
		bool		m_baseAnimEnable; ///< Base animation enable flag

		IMMDNode*	m_updateRoot; ///< Highest chain node used for final subtree refresh
		std::vector<IMMDNode*>	m_targetPath; ///< Nodes from update root to target
		std::vector<IMMDNode*>	m_ikPath; ///< Nodes from update root to IK node
		std::vector<size_t>		m_targetPathIndices; ///< Target-path index for each chain node
		std::vector<size_t>		m_ikPathIndices; ///< IK-path index for each chain node
	};
}

#endif // !LIBMMD_MODEL_MMD_MMDIKSOLVER_H_
