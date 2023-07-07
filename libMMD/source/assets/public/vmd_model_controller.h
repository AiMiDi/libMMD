/**************************************************************************

Copyright:Copyright(c) 2022-present, Aimidi & Walter White & CMT contributors.
Author:			Aimidi
Date:			2022/7/14
File:			vmd_model_controller_animation.h
Description:	MMD style model information animation

**************************************************************************/

#pragma once

#include "vmd_element.hpp"
#include "vmd_IK_controller_impl.h"

namespace libmmd
{
	class vmd_model_controller_key_frame_impl final : public vmd_model_controller_key_frame, public vmd_key_frame_impl
	{
		// Is model show
		bool				 model_show_;
		// IKs enable information
		vmd_element_array_impl<vmd_IK_controller, vmd_IK_controller_impl> IK_controller_array{};
	public:
		/**
		 * \brief Constructor function
		 */
		vmd_model_controller_key_frame_impl() = default;
		/**
		 * \brief Destructor function
		 */
		~vmd_model_controller_key_frame_impl() override = default;
		/**
		 * \brief Copy constructor
		 */
		vmd_model_controller_key_frame_impl(const vmd_model_controller_key_frame_impl&) noexcept = default;
		/**
		 * \brief Move constructor
		 */
		vmd_model_controller_key_frame_impl(vmd_model_controller_key_frame_impl&&) noexcept = default;
		/**
		* \brief Copy operator=
		* \return Result reference
		*/
		vmd_model_controller_key_frame_impl& operator =(const vmd_model_controller_key_frame_impl&) = default;
		/**
		 * \brief Move operator=
		 * \return Result reference
		 */
		vmd_model_controller_key_frame_impl& operator =(vmd_model_controller_key_frame_impl&&) noexcept = default;
		bool is_mode_show() const override;

		void set_mode_show(bool value) override;

		const vmd_IK_controller_array& get_vmd_IK_controller_array() override;

		vmd_IK_controller_array& mutable_vmd_IK_controller_array() override;
		bool read_from_file(const file& file) override;
		[[nodiscard]] bool write_to_file(const file& file) const override;
	};
}
