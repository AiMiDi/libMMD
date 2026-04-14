//
// Copyright(c) 2016-2019 benikabocha.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)
//

#ifndef LIBMMD_MODEL_MMD_IMMDNODE_H_
#define LIBMMD_MODEL_MMD_IMMDNODE_H_

#include <string>

#include <Eigen/Geometry>

namespace libmmd
{
	class IMMDNode
	{
	public:
		virtual ~IMMDNode() = default;

		virtual const std::string& GetName() const = 0;

		virtual void SetGlobalTransform(const Eigen::Matrix4f& m) = 0;
		virtual const Eigen::Matrix4f& GetGlobalTransform() const = 0;
		virtual const Eigen::Matrix4f& GetInitialGlobalTransform() const = 0;

		virtual const Eigen::Matrix4f& GetLocalTransform() const = 0;

		virtual void SetIKRotate(const Eigen::Quaternionf& ikr) = 0;
		virtual const Eigen::Quaternionf& GetIKRotate() const = 0;
		virtual Eigen::Quaternionf AnimateRotate() const = 0;

		virtual void UpdateLocalTransform() = 0;
		virtual void UpdateGlobalTransform() = 0;

		virtual IMMDNode* GetParent() const = 0;
	};
}

#endif // !LIBMMD_MODEL_MMD_IMMDNODE_H_
