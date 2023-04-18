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
	VMD::VMD(const std::u8string& model_name, const bool is_camera) :
		m_data(std::make_unique<data_type>(is_camera ? GetDefaultCameraName() : model_name)), is_camera_(is_camera) {}

	bool VMD::load_from_file(path& path)
	{
		file file;
		if (!path.check_suffix("vmd"))
			return false;

		if (!file.open(path, file::FILEOPEN::READ))
			return false;

		Char VMD_version[30]{ '\0' };
		Char VMD_model_name[20]{ '\0' };
		if (!file.read_elements(VMD_version, 30LLU))
			return false;
		if (!strncmp(VMD_version, "Vocaloid Motion Data 0002", 25LLU))
		{
			if (!file.read_elements(VMD_model_name, 20))
				return false;
			m_data->model_name = code_converter::shift_jis_to_utf8(VMD_model_name);
			// カメラ照明
			if (m_data->model_name.compare(0, 6, GetDefaultCameraName()) == 0)
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
			m_data->model_name = code_converter::shift_jis_to_utf8(VMD_model_name);
			if (m_data->model_name.compare(0, 6, GetDefaultCameraName()) == 0)
			{
				is_camera_ = true;
			}
			else {
				is_camera_ = false;
			}
		}
		else
		{
			MessageDialog("Import failed: File corruption or unknown version");
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

	bool VMD::save_to_file(path& path) const
	{
		file file;
		if (!path.check_suffix("vmd"))
			return false;

		if (!file.open(path, file::FILEOPEN::WRITE))
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
		if (!m_data->model_frames.write_to_file(file))
			return false;
		return true;
	}
}