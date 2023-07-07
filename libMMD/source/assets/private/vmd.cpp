/**************************************************************************

Copyright:Copyright(c) 2022-present, Aimidi & Walter White & CMT contributors.
Author:			Aimidi
Date:			2022/7/2
File:			vmd_data.cpp
Description:	vmd file data

**************************************************************************/

#include "vmd.h"

namespace libmmd
{
	bool vmd_animation_impl::read_from_file_impl(const path& path)
	{
		file file;
		if (!path.check_suffix("vmd"))
			return false;

		if (!file.open(path, file::open_mode::READ))
			return false;

		Char VMD_version[30]{ '\0' };
		Char VMD_model_name[20]{ '\0' };
		if (!file.read_elements(VMD_version, 30LLU))
			return false;
		if (!strncmp(VMD_version, "Vocaloid Motion Data 0002", 25LLU))
		{
			if (!file.read_elements(VMD_model_name, 20))
				return false;
			model_name_ = code_converter::shift_jis_to_utf8(VMD_model_name);
			// カメラ照明
			if(model_name_.compare(0, 6, GetDefaultCameraName()) == 0)
			{
				is_camera_ = true;
			}
			else {
				is_camera_ = false;
			}
		}
		else if (!strncmp(VMD_version, "Vocaloid Motion Data", 20LLU))
		{
			if (!file.read_elements(VMD_model_name, 10LLU))
				return false;
			model_name_ = code_converter::shift_jis_to_utf8(VMD_model_name);
			if (model_name_.compare(0, 6, GetDefaultCameraName()) == 0)
			{
				is_camera_ = true;
			}
			else {
				is_camera_ = false;
			}
		}
		else
		{
			ELOGD << "VMD::load_from_file() error at file corruption or unknown version";
			return false;
		}
		if (!m_data->motion_frames.read_from_file(file))
			return false;
		if (!m_data->morph_frames.read_from_file(file))
			return false;
		if (!m_data->camera_frames.read_from_file(file))
			return false;
		if (!m_data->light_frames.read_from_file(file))
			return false;
		if (!m_data->shadow_frames.read_from_file(file))
			return false;
		if (!m_data->model_frames.read_from_file(file))
			return false;
		return true;
	}

	bool vmd_animation_impl::save_to_file(path& path) const
	{
		file file;
		if (!path.check_suffix("vmd"))
			return false;

		if (!file.open(path, file::open_mode::WRITE))
			return false;

		if (constexpr Char vmd_version[30] = "Vocaloid Motion Data 0002"; !file.write_elements(vmd_version, 30LLU))
			return false;
		if (const std::string vmd_mode_name = code_converter::utf8_to_shift_jis(m_data->model_name);
			!file.write_elements(vmd_mode_name.data(), 20LLU))
			return false;
		if (!m_data->motion_frames.write_to_file(file))
			return false;
		if (!m_data->morph_frames.write_to_file(file))
			return false;
		if (!m_data->camera_frames.write_to_file(file))
			return false;
		if (!m_data->light_frames.write_to_file(file))
			return false;
		if (!m_data->shadow_frames.write_to_file(file))
			return false;
		if (!model_controller_frames_.write_to_file(file))
			return false;
		return true;
	}

	std::string vmd_animation_impl::get_model_name() const
	{
		return std::string{model_name_.begin(), model_name_.end()};
	}

	void vmd_animation_impl::set_model_name(const std::string& name)
	{
		model_name_ = std::u8string{ name.begin(), name.end() };
	}

	const vmd_animation::vmd_bone_key_frame_array& vmd_animation_impl::get_vmd_bone_key_frame_array()
	{
		return bone_frames_;
	}

	vmd_animation::vmd_bone_key_frame_array& vmd_animation_impl::mutable_vmd_bone_key_frame_array()
	{
		return bone_frames_;
	}

	const vmd_animation::vmd_morph_key_frame_array& vmd_animation_impl::get_vmd_morph_key_frame_array()
	{
		return morph_frames_;
	}

	vmd_animation::vmd_morph_key_frame_array& vmd_animation_impl::mutable_vmd_morph_key_frame_array()
	{
		return morph_frames_;
	}

	const vmd_animation::vmd_camera_key_frame_array& vmd_animation_impl::get_vmd_camera_key_frame_array()
	{
		return camera_frames_;
	}

	vmd_animation::vmd_camera_key_frame_array& vmd_animation_impl::mutable_vmd_camera_key_frame_array()
	{
		return camera_frames_;
	}

	const vmd_animation::vmd_light_key_frame_array& vmd_animation_impl::get_vmd_light_key_frame_array()
	{
		return light_frames_;
	}

	vmd_animation::vmd_light_key_frame_array& vmd_animation_impl::mutable_vmd_light_key_frame_array()
	{
		return light_frames_;
	}

	const vmd_animation::vmd_shadow_key_frame_array& vmd_animation_impl::get_vmd_shadow_key_frame_array()
	{
		return shadow_frames_;
	}

	vmd_animation::vmd_shadow_key_frame_array& vmd_animation_impl::mutable_vmd_shadow_key_frame_array()
	{
		return shadow_frames_;
	}

	const vmd_animation::vmd_model_controller_key_frame_array& vmd_animation_impl::
	get_vmd_model_controller_key_frame_array()
	{
		return model_controller_frames_;
	}

	vmd_animation::vmd_model_controller_key_frame_array& vmd_animation_impl::
	mutable_vmd_model_controller_key_frame_array()
	{
		return model_controller_frames_;
	}
}
