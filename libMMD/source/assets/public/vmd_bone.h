/**************************************************************************

Copyright:Copyright(c) 2022-present, Aimidi & Walter White & CMT contributors.
Author:			Aimidi
Date:			2022/7/16
File:			vmd_bone_animation.h
Description:	MMD style bone animation

**************************************************************************/

#pragma once

#include "vmd_element.hpp"
#include "vmd_interpolator.h"

namespace libmmd
{
	class VMDBoneAnimation
	{
		UInt32 frame_num_;
		// The action corresponds to the bone name
		std::u8string	bone_name_{ u8"bone" };
		// The action corresponds to the bone position
		Vector32		position_{};
		// The action corresponds to the rotation of the bone (quad)
		Vector4d32		rotation_{};
		// X-axis displacement action interpolation
		VMDBoneInterpolator interpolator_position_x_{};
		// Y-axis displacement action interpolation
		VMDBoneInterpolator interpolator_position_y_{};
		// Z-axis displacement action interpolation
		VMDBoneInterpolator interpolator_position_z_{};
		// Rotation action interpolation
		VMDBoneInterpolator interpolator_rotation_{};

		CMT_DEFAULT_COPY_BODY(VMDBoneAnimation)
		CMT_DEFAULT_MOVE_BODY(VMDBoneAnimation)
	public:
		/**
		 * \brief Constructor function
		 * \param bone_name The action corresponds to the bone name
		 * \param position The action corresponds to the bone position
		 * \param rotation The action corresponds to the rotation of the bone (quad)
		 * \param interpolator_position_x X-axis displacement action interpolation
		 * \param interpolator_position_y Y-axis displacement action interpolation
		 * \param interpolator_position_z Z-axis displacement action interpolation
		 * \param interpolator_rotation Rotation action interpolation
		 */
		explicit VMDBoneAnimation(
			std::u8string bone_name = {},
			const Vector32& position = {},
			const Vector4d32& rotation = {},
			VMDBoneInterpolator interpolator_position_x = {},
			VMDBoneInterpolator interpolator_position_y = {},
			VMDBoneInterpolator interpolator_position_z = {},
			VMDBoneInterpolator interpolator_rotation   = {}) :
			bone_name_(std::move(bone_name)),
			position_(position),
			rotation_(rotation),
			interpolator_position_x_(std::move(interpolator_position_x)),
			interpolator_position_y_(std::move(interpolator_position_y)),
			interpolator_position_z_(std::move(interpolator_position_z)),
			interpolator_rotation_(std::move(interpolator_rotation)) {}
		/**
		 * \brief Destructor function
		 */
		~VMDBoneAnimation() = default;
		/**
		 * \brief Read from a vmd file
		 * \param file vmd file
		 * \return Successful TRUE, other FALSE.
		 */
		bool read_from_file(const file& file);
		/**
		 * \brief Write to vmd file
		 * \param file vmd file
		 * \return Successful TRUE, other FALSE.
		 */
		bool write_to_file(const file& file) const;
		/**
		 * \brief Equality operator, Sort by frame order
		 * \param other Another instance
		 * \return true is returned if it is equal to another instance, and the other is false
		 */
		bool operator ==(const VMDBoneAnimation& other) const
		{
			return frame_num_ == other.frame_num_;
		}
		/**
		 * \brief Less than operator, Sort by frame order
		 * \param other Another instance
		 * \return true is returned if it is less than another instance, and the other is false
		 */
		bool operator <(const VMDBoneAnimation& other) const
		{
			return frame_num_ < other.frame_num_;
		}
	};
}
