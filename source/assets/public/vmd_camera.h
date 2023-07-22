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
	class vmd_camera_key_frame_impl : public vmd_camera_key_frame, public vmd_key_frame_impl
	{
		// Camera view distance
		Float32		distance_{};
		// View Angle
		UInt32		viewing_angle_{};
		// 0:on, 1:off
		UChar		perspective_{};
		// Camera view position of the camera
		Vector32	position_{};
		// Camera view rotation (Euler Angle of x and Y reversal)
		Vector32	rotation_{};
		// X-axis displacement action interpolation
		vmd_camera_interpolator_impl interpolator_position_x_{};
		// Y-axis displacement action interpolation
		vmd_camera_interpolator_impl interpolator_position_y_{};
		// Z-axis displacement action interpolation
		vmd_camera_interpolator_impl interpolator_position_z_{};
		// Rotation action interpolation
		vmd_camera_interpolator_impl interpolator_rotation_{};
		// Distance action interpolation
		vmd_camera_interpolator_impl interpolator_distance_{};
		// View Angle motion interpolation
		vmd_camera_interpolator_impl interpolator_viewing_angle_{};
	public:
		/**
		 * \brief Constructor function
		 */
		vmd_camera_key_frame_impl() = default;
		/**
		 * \brief Destructor function
		 */
		~vmd_camera_key_frame_impl() override = default;
		/**
		 * \brief Copy constructor
		 */
		vmd_camera_key_frame_impl(const vmd_camera_key_frame_impl&) noexcept = default;
		/**
		 * \brief Move constructor
		 */
		vmd_camera_key_frame_impl(vmd_camera_key_frame_impl&&) noexcept = default;
		/**
		* \brief Copy operator=
		* \return Result reference
		*/
		vmd_camera_key_frame_impl& operator =(const vmd_camera_key_frame_impl&) = default;
		/**
		 * \brief Move operator=
		 * \return Result reference
		 */
		vmd_camera_key_frame_impl& operator =(vmd_camera_key_frame_impl&&) noexcept = default;

		[[nodiscard]] float get_distance() const override;
		void set_distance(const float distance) override;

		[[nodiscard]] const std::array<float, 3>& get_position() const override;
		void set_position(const std::array<float, 3>& position) override;

		[[nodiscard]] const std::array<float, 3>& get_rotation() const override;
		void set_rotation(const std::array<float, 3>& rotation) override;

		[[nodiscard]] uint32_t get_view_angle() const override;
		void set_view_angle(const uint32_t view_angle) override;

		[[nodiscard]] bool is_perspective() const override;
		void set_perspective(bool value) override;

		[[nodiscard]] const vmd_interpolator& get_position_x_interpolator() const override;
		[[nodiscard]] vmd_interpolator& mutable_position_x_interpolator() override;

		[[nodiscard]] const vmd_interpolator& get_position_y_interpolator() const override;
		[[nodiscard]] vmd_interpolator& mutable_position_y_interpolator() override;

		[[nodiscard]] const vmd_interpolator& get_position_z_interpolator() const override;
		[[nodiscard]] vmd_interpolator& mutable_position_z_interpolator() override;

		[[nodiscard]] const vmd_interpolator& get_rotation_interpolator() const override;
		[[nodiscard]] vmd_interpolator& mutable_rotation_interpolator() override;

		[[nodiscard]] const vmd_interpolator& get_distance_interpolator() const override;
		[[nodiscard]] vmd_interpolator& mutable_distance_interpolator() override;

		[[nodiscard]] const vmd_interpolator& get_view_angle_interpolator() const override;
		[[nodiscard]] vmd_interpolator& mutable_view_angle_interpolator() override;

		bool read_from_file(const file& file) override;
		bool write_to_file(const file& file) const override;
	};
}