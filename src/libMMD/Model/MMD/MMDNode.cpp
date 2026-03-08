//
// Copyright(c) 2016-2017 benikabocha.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)
//

#include <stack>
#include "MMDNode.h"
#include <libMMD/Base/Log.h>

namespace libmmd
{
	MMDNode::MMDNode()
		: m_index(0)
		, m_enableIK(false)
		, m_parent(nullptr)
		, m_child(nullptr)
		, m_next(nullptr)
		, m_prev(nullptr)
		, m_translate(Eigen::Vector3f::Zero())
		, m_rotate(Eigen::Quaternionf::Identity())
		, m_scale(Eigen::Vector3f::Ones())
		, m_animTranslate(Eigen::Vector3f::Zero())
		, m_animRotate(Eigen::Quaternionf::Identity())
		, m_baseAnimTranslate(Eigen::Vector3f::Zero())
		, m_baseAnimRotate(Eigen::Quaternionf::Identity())
		, m_ikRotate(Eigen::Quaternionf::Identity())
		, m_local(Eigen::Matrix4f::Identity())
		, m_global(Eigen::Matrix4f::Identity())
		, m_inverseInit(Eigen::Matrix4f::Identity())
		, m_initTranslate(Eigen::Vector3f::Zero())
		, m_initRotate(Eigen::Quaternionf::Identity())
		, m_initScale(Eigen::Vector3f::Ones())
	{
	}

	void MMDNode::AddChild(MMDNode * child)
	{
		LIBMMD_ASSERT(child != nullptr);
		if (child == nullptr)
		{
			return;
		}

		LIBMMD_ASSERT(child->m_parent == nullptr);
		LIBMMD_ASSERT(child->m_next == nullptr);
		LIBMMD_ASSERT(child->m_prev == nullptr);
		child->m_parent = this;
		if (m_child == nullptr)
		{
			m_child = child;
			m_child->m_next = nullptr;
			m_child->m_prev = m_child;
		}
		else
		{
			const auto lastNode = m_child->m_prev;
			lastNode->m_next = child;
			child->m_prev = lastNode;

			m_child->m_prev = child;
		}
	}

	void MMDNode::BeginUpdateTransform()
	{
		LoadInitialTRS();
		SetIKRotate(Eigen::Quaternionf::Identity());
		OnBeginUpdateTransform();
	}

	void MMDNode::EndUpdateTransform()
	{
		OnEndUpdateTransfrom();
	}

	void MMDNode::UpdateLocalTransform()
	{
		OnUpdateLocalTransform();
	}

void MMDNode::UpdateGlobalTransform()
{
	std::stack<MMDNode*> nodeStack;
	nodeStack.push(this);

	while (!nodeStack.empty())
	{
		MMDNode* currentNode = nodeStack.top();
		nodeStack.pop();

		if (currentNode->m_parent == nullptr)
		{
			currentNode->m_global = currentNode->m_local;
		}
		else
		{
			currentNode->m_global = currentNode->m_parent->m_global * currentNode->m_local;
		}

		MMDNode* child = currentNode->m_child;
		while (child != nullptr)
		{
			nodeStack.push(child);
			child = child->m_next;
		}
	}
}

	void MMDNode::UpdateChildTransform() const
	{
		MMDNode* child = m_child;
		while (child != nullptr)
		{
			child->UpdateGlobalTransform();
			child = child->m_next;
		}
	}

	void MMDNode::CalculateInverseInitTransform()
	{
		m_inverseInit = m_global.inverse();
	}

	void MMDNode::OnBeginUpdateTransform()
	{
	}

	void MMDNode::OnEndUpdateTransfrom()
	{
	}

	void MMDNode::OnUpdateLocalTransform()
	{
		const Eigen::Quaternionf rot = m_enableIK
			? (m_ikRotate * AnimateRotate())
			: AnimateRotate();
		const Eigen::Vector3f& sc = GetScale();

		m_local.setIdentity();
		m_local.block<3,3>(0,0).noalias() = rot.toRotationMatrix() * sc.asDiagonal();
		m_local.block<3,1>(0,3) = AnimateTranslate();
	}

}
