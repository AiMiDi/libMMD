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
	class vmd_bone_key_frame_impl final : public vmd_bone_key_frame, public vmd_key_frame_impl
	{
		// The action corresponds to the bone name
		std::u8string	bone_name_{ u8"bone" };
		// The action corresponds to the bone position
		Vector32		position_{};
		// The action corresponds to the rotation of the bone (quad)
		Vector4d32		rotation_{};
		// X-axis displacement action interpolation
		vmd_bone_interpolator_impl interpolator_position_x_{};
		// Y-axis displacement action interpolation
		vmd_bone_interpolator_impl interpolator_position_y_{};
		// Z-axis displacement action interpolation
		vmd_bone_interpolator_impl interpolator_position_z_{};
		// Rotation action interpolation
		vmd_bone_interpolator_impl interpolator_rotation_{};
	public:
		/**
		 * \brief Constructor function
		 */
		vmd_bone_key_frame_impl() = default;
		/**
		 * \brief Destructor function
		 */
		~vmd_bone_key_frame_impl() override = default;
		/**
		 * \brief Copy constructor
		 */
		vmd_bone_key_frame_impl(const vmd_bone_key_frame_impl&) noexcept = default;
		/**
		 * \brief Move constructor
		 */
		vmd_bone_key_frame_impl(vmd_bone_key_frame_impl&&) noexcept = default; 
		/**
		* \brief Copy operator=
		* \return Result reference
		*/
		vmd_bone_key_frame_impl& operator =(const vmd_bone_key_frame_impl&) = default;
		/**
		 * \brief Move operator=
		 * \return Result reference
		 */
		vmd_bone_key_frame_impl& operator =(vmd_bone_key_frame_impl&&) noexcept = default;

		using vmd_key_frame_impl::get_frame_at;
		using vmd_key_frame_impl::set_frame_at;

		[[nodiscard]] std::string get_bone_name() const override;
		void set_bone_name(const std::string& name) override;

		[[nodiscard]] const std::array<float, 3>& get_position() const override;
		void set_position(const std::array<float, 3>& position) override;

		[[nodiscard]] const std::array<float, 4>& get_rotation() const override;
		void set_rotation(const std::array<float, 4>& rotation) override;

		[[nodiscard]] const vmd_interpolator& get_position_x_interpolator() const override;
		vmd_interpolator& mutable_position_x_interpolator() override;

		[[nodiscard]] const vmd_interpolator& get_position_y_interpolator() const override;
		vmd_interpolator& mutable_position_y_interpolator() override;

		[[nodiscard]] const vmd_interpolator& get_position_z_interpolator() const override;
		vmd_interpolator& mutable_position_z_interpolator() override;

		[[nodiscard]] const vmd_interpolator& get_rotation_interpolator() const override;
		vmd_interpolator& mutable_rotation_interpolator() override;

		bool read_from_file(const file& file) override;
		[[nodiscard]] bool write_to_file(const file& file) const override;
	};
}
