/**************************************************************************

Copyright:Copyright(c) 2022-present, Aimidi & Walter White & CMT contributors.
Author:			Aimidi
Date:			2022/7/15
File:			vmd_light_animation.h
Description:	MMD style lighting animation

**************************************************************************/

#pragma once

#include "vmd_element.hpp"

namespace libmmd
{
	class vmd_light_key_frame_impl final : public vmd_light_key_frame, public vmd_key_frame_impl
	{
		// Light color
		Vector32	color_{};
		// location
		Vector32	position_{};
	public:
		/**
		 * \brief Constructor function
		 */
		vmd_light_key_frame_impl() = default;
		/**
		 * \brief Destructor function
		 */
		~vmd_light_key_frame_impl() override = default;
		/**
		 * \brief Copy constructor
		 */
		vmd_light_key_frame_impl(const vmd_light_key_frame_impl&) noexcept = default;
		/**
		 * \brief Move constructor
		 */
		vmd_light_key_frame_impl(vmd_light_key_frame_impl&&) noexcept = default;
		/**
		* \brief Copy operator=
		* \return Result reference
		*/
		vmd_light_key_frame_impl& operator =(const vmd_light_key_frame_impl&) = default;
		/**
		 * \brief Move operator=
		 * \return Result reference
		 */
		vmd_light_key_frame_impl& operator =(vmd_light_key_frame_impl&&) noexcept = default;

		const std::array<float, 3>& get_color() const override;

		void set_color(const std::array<float, 3>& color) override;

		const std::array<float, 3>& get_position() const override;

		void set_position(const std::array<float, 3>& position) override;
		bool read_from_file(const file& file) override;
		[[nodiscard]] bool write_to_file(const file& file) const override;
	};
}
