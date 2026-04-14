//
// Copyright(c) 2016-2017 benikabocha.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)
//

#ifndef LIBMMD_MODEL_MMD_MMDNODE_H_
#define LIBMMD_MODEL_MMD_MMDNODE_H_

#include "IMMDNode.h"

#include <string>
#include <Eigen/Geometry>

namespace libmmd
{
	/**
	 * @brief Represents a node in an MMD model.
	 */
	class MMDNode : public IMMDNode
	{
	public:
		EIGEN_MAKE_ALIGNED_OPERATOR_NEW

		MMDNode();
		~MMDNode() override = default;

		/**
		 * @brief Add a child node.
		 * @param child The child node to add.
		 */
		void AddChild(MMDNode* child);

		/**
		 * @brief Called before updating the transform.
		 */
		void BeginUpdateTransform();

		/**
		 * @brief Called after updating the transform.
		 */
		void EndUpdateTransform();

		/**
		 * @brief Update the local transform.
		 */
		void UpdateLocalTransform() override;

		/**
		 * @brief Update the global transform.
		 */
		void UpdateGlobalTransform() override;

		/**
		 * @brief Update the transform of child nodes.
		 */
		void UpdateChildTransform() const;

		/**
		 * @brief Set the index of the node.
		 * @param idx The index of the node.
		 */
		void SetIndex(const uint32_t idx) { m_index = idx; }

		/**
		 * @brief Get the index of the node.
		 * @return The index of the node.
		 */
		uint32_t GetIndex() const { return m_index; }

		/**
		 * @brief Set the name of the node.
		 * @param name The name of the node.
		 */
		void SetName(const std::string& name) { m_name = name; }

		/**
		 * @brief Get the name of the node.
		 * @return The name of the node.
		 */
		const std::string& GetName() const override { return m_name; }

		/**
		 * @brief Enable or disable IK for the node.
		 * @param enable Whether to enable IK.
		 */
		void EnableIK(const bool enable) { m_enableIK = enable; }

		/**
		 * @brief Check if IK is enabled for the node.
		 * @return True if IK is enabled, false otherwise.
		 */
		bool IsIK() const { return m_enableIK; }

		/**
		 * @brief Set the translation of the node.
		 * @param t The translation vector.
		 */
		void SetTranslate(const Eigen::Vector3f& t) { m_translate = t; }

		/**
		 * @brief Get the translation of the node.
		 * @return The translation vector.
		 */
		const Eigen::Vector3f& GetTranslate() const { return m_translate; }

		/**
		 * @brief Set the rotation of the node.
		 * @param r The rotation quaternion.
		 */
		void SetRotate(const Eigen::Quaternionf& r) { m_rotate = r; }

		/**
		 * @brief Get the rotation of the node.
		 * @return The rotation quaternion.
		 */
		const Eigen::Quaternionf& GetRotate() const { return m_rotate; }

		/**
		 * @brief Set the scale of the node.
		 * @param s The scale vector.
		 */
		void SetScale(const Eigen::Vector3f& s) { m_scale = s; }

		/**
		 * @brief Get the scale of the node.
		 * @return The scale vector.
		 */
		const Eigen::Vector3f& GetScale() const { return m_scale; }

		/**
		 * @brief Set the animation translation of the node.
		 * @param t The animation translation vector.
		 */
		void SetAnimationTranslate(const Eigen::Vector3f& t) { m_animTranslate = t; }

		/**
		 * @brief Get the animation translation of the node.
		 * @return The animation translation vector.
		 */
		const Eigen::Vector3f& GetAnimationTranslate() const { return m_animTranslate; }

		/**
		 * @brief Set the animation rotation of the node.
		 * @param q The animation rotation quaternion.
		 */
		void SetAnimationRotate(const Eigen::Quaternionf& q) { m_animRotate = q; }

		/**
		 * @brief Get the animation rotation of the node.
		 * @return The animation rotation quaternion.
		 */
		const Eigen::Quaternionf& GetAnimationRotate() const { return m_animRotate; }

		/**
		 * @brief Get the combined translation for animation.
		 * @return The combined translation vector.
		 */
		Eigen::Vector3f AnimateTranslate() const { return m_animTranslate + m_translate; }

		/**
		 * @brief Get the combined rotation for animation.
		 * @return The combined rotation quaternion.
		 */
		Eigen::Quaternionf AnimateRotate() const override { return m_animRotate * m_rotate; }

		/**
		 * @brief Set the IK rotation of the node.
		 * @param ikr The IK rotation quaternion.
		 */
		void SetIKRotate(const Eigen::Quaternionf& ikr) override { m_ikRotate = ikr; }

		/**
		 * @brief Get the IK rotation of the node.
		 * @return The IK rotation quaternion.
		 */
		const Eigen::Quaternionf& GetIKRotate() const override { return m_ikRotate; }

		/**
		 * @brief Get the parent node.
		 * @return A pointer to the parent node.
		 */
		MMDNode* GetParent() const override { return m_parent; }

		/**
		 * @brief Get the child node.
		 * @return A pointer to the child node.
		 */
		MMDNode* GetChild() const { return m_child; }

		/**
		 * @brief Get the next sibling node.
		 * @return A pointer to the next sibling node.
		 */
		MMDNode* GetNext() const { return m_next; }

		/**
		 * @brief Get the previous sibling node.
		 * @return A pointer to the previous sibling node.
		 */
		MMDNode* GetPrev() const { return m_prev; }

		/**
		 * @brief Set the local transform matrix.
		 * @param m The local transform matrix.
		 */
		void SetLocalTransform(const Eigen::Matrix4f& m) { m_local = m; }

		/**
		 * @brief Get the local transform matrix.
		 * @return The local transform matrix.
		 */
		const Eigen::Matrix4f& GetLocalTransform() const override { return m_local; }

		/**
		 * @brief Set the global transform matrix.
		 * @param m The global transform matrix.
		 */
		void SetGlobalTransform(const Eigen::Matrix4f& m) override { m_global = m; }

		/**
		 * @brief Get the global transform matrix.
		 * @return The global transform matrix.
		 */
		const Eigen::Matrix4f& GetGlobalTransform() const override { return m_global; }

		/**
		 * @brief Get the captured bind-pose global transform matrix.
		 * @return The initial global transform matrix.
		 */
		const Eigen::Matrix4f& GetInitialGlobalTransform() const override { return m_initialGlobal; }

		/**
		 * @brief Calculate the inverse of the initial transform.
		 */
		void CalculateInverseInitTransform();
		
		/**
		 * @brief Get the inverse of the initial transform.
		 * @return The inverse of the initial transform matrix.
		 */
		const Eigen::Matrix4f& GetInverseInitTransform() const { return m_inverseInit; }

		/**
		 * @brief Save the initial translation, rotation, and scale.
		 */
		void SaveInitialTRS()
		{
			m_initTranslate = m_translate;
			m_initRotate = m_rotate;
			m_initScale = m_scale;
		}

		/**
		 * @brief Load the initial translation, rotation, and scale.
		 */
		void LoadInitialTRS()
		{
			m_translate = m_initTranslate;
			m_rotate = m_initRotate;
			m_scale = m_initScale;
			//m_animTranslate = Eigen::Vector3f::Zero();
			//m_animRotate = Eigen::Quaternionf::Identity();
		}

		/**
		 * @brief Get the initial translation.
		 * @return The initial translation vector.
		 */
		const Eigen::Vector3f& GetInitialTranslate() const { return m_initTranslate; }

		/**
		 * @brief Get the initial rotation.
		 * @return The initial rotation quaternion.
		 */
		const Eigen::Quaternionf& GetInitialRotate() const { return m_initRotate; }

		/**
		 * @brief Get the initial scale.
		 * @return The initial scale vector.
		 */
		const Eigen::Vector3f& GetInitialScale() const { return m_initScale; }

		/**
		 * @brief Save the base animation state.
		 */
		void SaveBaseAnimation()
		{
			m_baseAnimTranslate = m_animTranslate;
			m_baseAnimRotate = m_animRotate;
		}

		/**
		 * @brief Load the base animation state.
		 */
		void LoadBaseAnimation()
		{
			m_animTranslate = m_baseAnimTranslate;
			m_animRotate = m_baseAnimRotate;
		}

		/**
		 * @brief Clear the base animation state.
		 */
		void ClearBaseAnimation()
		{
			m_baseAnimTranslate = Eigen::Vector3f::Zero();
			m_baseAnimRotate = Eigen::Quaternionf::Identity();
		}

		/**
		 * @brief Get the base animation translation.
		 * @return The base animation translation vector.
		 */
		const Eigen::Vector3f& GetBaseAnimationTranslate() const { return m_baseAnimTranslate; }

		/**
		 * @brief Get the base animation rotation.
		 * @return The base animation rotation quaternion.
		 */
		const Eigen::Quaternionf& GetBaseAnimationRotate() const { return m_baseAnimRotate; }

	protected:
		/**
		 * @brief Called before updating the transform.
		 */
		virtual void OnBeginUpdateTransform();

		/**
		 * @brief Called after updating the transform.
		 */
		virtual void OnEndUpdateTransfrom();

		/**
		 * @brief Called to update the local transform.
		 */
		virtual void OnUpdateLocalTransform();

		uint32_t		m_index;
		std::string		m_name;
		bool			m_enableIK;

		MMDNode*		m_parent;
		MMDNode*		m_child;
		MMDNode*		m_next;
		MMDNode*		m_prev;

		Eigen::Vector3f	m_translate;
		Eigen::Quaternionf	m_rotate;
		Eigen::Vector3f	m_scale;

		Eigen::Vector3f	m_animTranslate;
		Eigen::Quaternionf	m_animRotate;

		Eigen::Vector3f	m_baseAnimTranslate;
		Eigen::Quaternionf	m_baseAnimRotate;

		Eigen::Quaternionf	m_ikRotate;

		Eigen::Matrix4f		m_local;
		Eigen::Matrix4f		m_global;
		Eigen::Matrix4f		m_initialGlobal;
		Eigen::Matrix4f		m_inverseInit;

		Eigen::Vector3f	m_initTranslate;
		Eigen::Quaternionf	m_initRotate;
		Eigen::Vector3f	m_initScale;
	};
}

#endif // !LIBMMD_MODEL_MMD_MMDNODE_H_

