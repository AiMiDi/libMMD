/**************************************************************************

Copyright:Copyright(c) 2022-present, Aimidi & Walter White & CMT contributors.
Author:			Aimidi
Date:			2022/7/14
File:			vmd_Ik_controller_animation.h
Description:	MMD style IK enable information animation

**************************************************************************/

#pragma once

#include "vmd_element.hpp"

namespace libmmd
{
	class vmd_IK_controller_impl final : public vmd_IK_controller, public vmd_key_frame_impl
	{
		// IK name
		std::u8string	IK_bone_name_{};
		// Is IK enable
		bool			IK_enable_{};
	public:
		/**
		 * \brief Constructor function
		 */
		vmd_IK_controller_impl() = default;
		/**
		 * \brief Destructor function
		 */
		~vmd_IK_controller_impl() override = default;
		/**
		 * \brief Copy constructor
		 */
		vmd_IK_controller_impl(const vmd_IK_controller_impl&) noexcept = default;
		/**
		 * \brief Move constructor
		 */
		vmd_IK_controller_impl(vmd_IK_controller_impl&&) noexcept = default;
		/**
		* \brief Copy operator=
		* \return Result reference
		*/
		vmd_IK_controller_impl& operator =(const vmd_IK_controller_impl&) = default;
		/**
		 * \brief Move operator=
		 * \return Result reference
		 */
		vmd_IK_controller_impl& operator =(vmd_IK_controller_impl&&) noexcept = default;

		std::string get_bone_name() const override;

		void set_bone_name(const std::string& name) override;

		bool is_IK_enable() const override;

		void set_IK_enable(bool value) override;

		bool read_from_file(const file& file) override;
		[[nodiscard]] bool write_to_file(const file& file) const override;
	};
}
