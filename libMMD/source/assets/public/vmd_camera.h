/**************************************************************************

Copyright:Copyright(c) 2022-present, Aimidi & Walter White & CMT contributors.
Author:			Aimidi
Date:			2022/7/16
File:			vmd_camera.h
Description:	

**************************************************************************/

#pragma once

#include "vmd_element.hpp"
#include "vmd_interpolator.h"

namespace libmmd
{
	/* MMD风格的摄像机动画 */
	class VMDCameraAnimation final : public vmd_element
	{
		// Camera view distance
		Float32		distance_{};
		// Camera view position of the camera
		Vector32	position_{};
		// Camera view rotation (Euler Angle of x and Y reversal)
		Vector32	rotation_{};
		// X-axis displacement action interpolation
		VMDCameraInterpolator interpolator_position_x_{};
		// Y-axis displacement action interpolation
		VMDCameraInterpolator interpolator_position_y_{};
		// Z-axis displacement action interpolation
		VMDCameraInterpolator interpolator_position_z_{};
		// Rotation action interpolation
		VMDCameraInterpolator interpolator_rotation_{};
		// Distance action interpolation
		VMDCameraInterpolator interpolator_distance_{};
		// View Angle motion interpolation
		VMDCameraInterpolator interpolator_viewing_angle_{};
		// View Angle
		UInt32		viewing_angle_{};
		// 0:on, 1:off
		UChar		perspective_{};

		CMT_DEFAULT_COPY_BODY(VMDCameraAnimation)
		CMT_DEFAULT_MOVE_BODY(VMDCameraAnimation)
	public:
		/**
		 * \brief Constructor function
		 * \param distance Camera view distance
		 * \param position Camera view position of the camera
		 * \param rotation Camera view rotation (Euler Angle of x and Y reversal)
		 * \param interpolator_position_x X-axis displacement action interpolation
		 * \param interpolator_position_y Y-axis displacement action interpolation
		 * \param interpolator_position_z Z-axis displacement action interpolation
		 * \param interpolator_rotation Rotation action interpolation
		 * \param interpolator_position_d Distance action interpolation
		 * \param interpolator_position_v View Angle motion interpolation
		 */
		explicit VMDCameraAnimation(
			const Float32& distance = {},
			const Vector32& position = {},
			const Vector32& rotation = {},
			VMDCameraInterpolator interpolator_position_x = {},
			VMDCameraInterpolator interpolator_position_y = {},
			VMDCameraInterpolator interpolator_position_z = {},
			VMDCameraInterpolator interpolator_rotation	  = {},
			VMDCameraInterpolator interpolator_position_d = {},
			VMDCameraInterpolator interpolator_position_v = {}) :
			distance_(distance),
			position_(position),
			rotation_(rotation),
			interpolator_position_x_(std::move(interpolator_position_x)),
			interpolator_position_y_(std::move(interpolator_position_y)),
			interpolator_position_z_(std::move(interpolator_position_z)),
			interpolator_rotation_(std::move(interpolator_rotation)),
			interpolator_distance_(std::move(interpolator_position_d)),
			interpolator_viewing_angle_(std::move(interpolator_position_v)) {}
		/**
		 * \brief Destructor function
		 */
		~VMDCameraAnimation() override = default;
		/**
		 * \brief Read from a vmd file
		 * \param file vmd file
		 * \return Successful TRUE, other FALSE.
		 */
		bool read_from_file(const file& file) override;
		/**
		 * \brief Write to vmd file
		 * \param file vmd file
		 * \return Successful TRUE, other FALSE.
		 */
		bool write_to_file(const file& file) const override;
	};
}