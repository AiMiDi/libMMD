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
		bone_name_ = code_converter::shift_jis_to_utf8(bone_name);
		if (!file.read_elements(frame_num_))
			return false;
		if (!file.read_elements(position_))
			return false;
		if (!file.read_elements(rotation_))
			return false;
		if (!interpolator_position_x_.read(file))
			return false;
		if (!interpolator_position_y_.read(file))
			return false;
		if (!interpolator_position_z_.read(file))
			return false;
		if (!interpolator_rotation_.read(file))
			return false;
		return true;
	}

	bool VMDBoneAnimation::write_to_file(const file& file) const
	{
		std::string bone_name = code_converter::utf8_to_shift_jis(bone_name_);
		bone_name.resize(15, '\0');
		if (!file.write_elements(bone_name.data(), 15LLU))
			return false;
		if (!file.write_elements(frame_num_))
			return false;
		if (!file.write_elements(position_))
			return false;
		if (!file.write_elements(rotation_, sizeof Vector4d32))
			return false;
		if (!interpolator_position_x_.write(file))
			return false;
		if (!interpolator_position_y_.write(file))
			return false;
		if (!interpolator_position_z_.write(file))
			return false;
		if (!interpolator_rotation_.write(file))
			return false;
		return true;
	}
}