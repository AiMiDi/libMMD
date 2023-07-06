/**************************************************************************

Copyright:Copyright(c) 2022-present, Aimidi & Walter White & CMT contributors.
Author:			Aimidi
Date:			2022/7/15
File:			vmd_shadow_animation.h
Description:	MMD style shadow animation

**************************************************************************/

#pragma once

#include "vmd_element.hpp"

namespace libmmd
{
	class vmd_shadow_key_frame_impl : public vmd_shadow_key_frame, public vmd_key_frame_impl
	{
		// 0:Off 1:mode1 2:mode2
		shadow_type	shadow_type_{};
		// Distance
		Float32 distance_{};
	public:
		/**
		 * \brief Constructor function
		 */
		vmd_shadow_key_frame_impl() = default;
		/**
		 * \brief Destructor function
		 */
		~vmd_shadow_key_frame_impl() override = default;
		/**
		 * \brief Copy constructor
		 */
		vmd_shadow_key_frame_impl(const vmd_shadow_key_frame_impl&) noexcept = default;
		/**
		 * \brief Move constructor
		 */
		vmd_shadow_key_frame_impl(vmd_shadow_key_frame_impl&&) noexcept = default;
		/**
		* \brief Copy operator=
		* \return Result reference
		*/
		vmd_shadow_key_frame_impl& operator =(const vmd_shadow_key_frame_impl&) = default;
		/**
		 * \brief Move operator=
		 * \return Result reference
		 */
		vmd_shadow_key_frame_impl& operator =(vmd_shadow_key_frame_impl&&) noexcept = default;

		shadow_type get_shadow_type() const override;
		void set_shadow_type(const shadow_type type) override;

		float get_distance() const override;
		void set_distance(float distance) override;

		bool read_from_file(const file& file) override;
		[[nodiscard]] bool write_to_file(const file& file) const override;
	};
}
