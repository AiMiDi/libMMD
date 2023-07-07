/**************************************************************************

Copyright:Copyright(c) 2022-present, Aimidi & Walter White & CMT contributors.
Author:			Aimidi
Date:			2022/7/2
File:			vmd_data.h
Description:	vmd file data

**************************************************************************/

#pragma once

#include "vmd_bone.h"
#include "vmd_camera.h"
#include "vmd_light.h"
#include "vmd_morph.h"
#include "vmd_shadow.h"
#include "vmd_model_controller.h"
#include "vmd_element.hpp"

namespace libmmd
{
	class vmd_animation_impl : public vmd_animation
	{
		std::u8string model_name;
		vmd_element_array_impl<vmd_bone_key_frame, vmd_bone_key_frame_impl> motion_frames{};
		vmd_element_array_impl<vmd_morph_key_frame, vmd_morph_key_frame_impl> morph_frames{};
		vmd_element_array_impl<vmd_camera_key_frame, vmd_camera_key_frame_impl> camera_frames{};
		vmd_element_array_impl<vmd_light_key_frame, vmd_light_key_frame_impl> light_frames{};
		vmd_element_array_impl<vmd_shadow_key_frame, vmd_shadow_key_frame_impl> shadow_frames{};
		vmd_element_array_impl<vmd_model_controller_key_frame, vmd_model_controller_key_frame_impl> model_frames{};
	public:
		/**
		 * \brief Constructor function
		 */
		vmd_animation_impl() = default;
		/**
		 * \brief Destructor function
		 */
		~vmd_animation_impl() override = default;
		/**
		 * \brief Copy constructor
		 */
		vmd_animation_impl(const vmd_animation_impl&) noexcept = default;
		/**
		 * \brief Move constructor
		 */
		vmd_animation_impl(vmd_animation_impl&&) noexcept = default;
		/**
		* \brief Copy operator=
		* \return Result reference
		*/
		vmd_animation_impl& operator =(const vmd_animation_impl&) = default;
		/**
		 * \brief Move operator=
		 * \return Result reference
		 */
		vmd_animation_impl& operator =(vmd_animation_impl&&) noexcept = default;
		std::string get_model_name() const override;
		void set_model_name(const std::string& name) override;
		const vmd_bone_key_frame_array& get_vmd_bone_key_frame_array() override;
		vmd_bone_key_frame_array& mutable_vmd_bone_key_frame_array() override;
		const vmd_morph_key_frame_array& get_vmd_morph_key_frame_array() override;
		vmd_morph_key_frame_array& mutable_vmd_morph_key_frame_array() override;
		const vmd_camera_key_frame_array& get_vmd_camera_key_frame_array() override;
		vmd_camera_key_frame_array& mutable_vmd_camera_key_frame_array() override;
		const vmd_light_key_frame_array& get_vmd_light_key_frame_array() override;
		vmd_light_key_frame_array& mutable_vmd_light_key_frame_array() override;
		const vmd_shadow_key_frame_array& get_vmd_shadow_key_frame_array() override;
		vmd_shadow_key_frame_array& mutable_vmd_shadow_key_frame_array() override;
		const vmd_model_controller_key_frame_array& get_vmd_model_controller_key_frame_array() override;
		vmd_model_controller_key_frame_array& mutable_vmd_model_controller_key_frame_array() override;
	};
}
