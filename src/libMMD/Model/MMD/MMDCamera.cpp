//
// Copyright(c) 2016-2017 benikabocha.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)
//

#include "MMDCamera.h"
#include <Eigen/Geometry>
#include <cmath>

namespace libmmd
{
	MMDCamera::MMDCamera()
	{
		m_interest = Eigen::Vector3f(0, 10, 0);
		m_rotate = Eigen::Vector3f::Zero();
		m_distance = 50;
		m_fov = 30.0f * static_cast<float>(EIGEN_PI) / 180.0f;
	}

	MMDLookAtCamera::MMDLookAtCamera(const MMDCamera & cam)
	{
		Eigen::Matrix4f view = Eigen::Matrix4f::Identity();
		view.block<3,1>(0,3) = Eigen::Vector3f(0, 0, std::abs(cam.m_distance));
		//view = mat4_cast(cam.m_rotate) * view;
		//Eigen::Matrix4f rot;
		//auto degree = degrees(cam.m_rotate);
		Eigen::Matrix4f rot = Eigen::Matrix4f::Identity();
		Eigen::Matrix4f rotY = Eigen::Matrix4f::Identity();
		rotY.block<3,3>(0,0) = Eigen::AngleAxisf(cam.m_rotate.y(), Eigen::Vector3f::UnitY()).toRotationMatrix();
		rot = rotY * rot;
		Eigen::Matrix4f rotZ = Eigen::Matrix4f::Identity();
		rotZ.block<3,3>(0,0) = Eigen::AngleAxisf(cam.m_rotate.z(), Eigen::Vector3f(0, 0, -1)).toRotationMatrix();
		rot = rotZ * rot;
		Eigen::Matrix4f rotX = Eigen::Matrix4f::Identity();
		rotX.block<3,3>(0,0) = Eigen::AngleAxisf(cam.m_rotate.x(), Eigen::Vector3f::UnitX()).toRotationMatrix();
		rot = rotX * rot;
		view = rot * view;

		m_eye = view.col(3).head<3>() + cam.m_interest;
		m_center = view.block<3,3>(0,0) * Eigen::Vector3f(0, 0, -1) + m_eye;
		m_up = view.block<3,3>(0,0) * Eigen::Vector3f(0, 1, 0);
	}
}
