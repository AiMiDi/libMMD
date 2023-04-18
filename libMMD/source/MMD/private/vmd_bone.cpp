/**************************************************************************

Copyright:Copyright(c) 2022-present, Aimidi & Walter White & CMT contributors.
Author:			Aimidi
Date:			2022/7/16
File:			vmd_bone_animation.cpp
Description:	MMD style bone animation

**************************************************************************/

#include "vmd_bone.h"

namespace libmmd
{
	bool VMDBoneAnimation::read_from_file(const file& file)
	{
		Char bone_name[15] {'\0'};
		if (!file.read_elements(bone_name, 15LLU))
			return false;
		m_data->m_bone_name = code_converter::shift_jis_to_utf8(bone_name);
		if (!file.read_elements(frame_num_))
			return false;
		if (!file.read_elements(m_data->m_position))
			return false;
		if (!file.read_elements(m_data->m_rotation))
			return false;
		if (!m_data->m_interpolator_position_x.read(file))
			return false;
		if (!m_data->m_interpolator_position_y.read(file))
			return false;
		if (!m_data->m_interpolator_position_z.read(file))
			return false;
		if (!m_data->m_interpolator_rotation.read(file))
			return false;
		return true;
	}

	bool VMDBoneAnimation::write_to_file(const file& file) const
	{
		std::string bone_name = code_converter::utf8_to_shift_jis(m_data->m_bone_name);
		bone_name.resize(15, '\0');
		if (!file.write_elements(bone_name.data(), 15LLU))
			return false;
		if (!file.write_elements(frame_num_))
			return false;
		if (!file.write_elements(m_data->m_position))
			return false;
		if (!file.write_elements(m_data->m_rotation, sizeof Vector4d32))
			return false;
		if (!m_data->m_interpolator_position_x.write(file))
			return false;
		if (!m_data->m_interpolator_position_y.write(file))
			return false;
		if (!m_data->m_interpolator_position_z.write(file))
			return false;
		if (!m_data->m_interpolator_rotation.write(file))
			return false;
		return true;
	}
}