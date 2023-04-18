/**************************************************************************

Copyright:Copyright(c) 2022-present, Aimidi & Walter White & CMT contributors.
Author:			Aimidi
Date:			2022/7/14
File:			vmd_Ik_controller_animation.cpp
Description:	MMD style IK enable information animation

**************************************************************************/

#include "vmd_Ik_controller.h"

namespace libmmd
{
	bool VMDIkController::read_from_file(const file& file)
	{
		Char ik_name[20]{ '\0' };
		if (!file.read_elements(ik_name, 20LLU))
			return false;
		if (!file.read_elements(m_data->IK_enable))
			return false;
		m_data->IK_name = code_converter::shift_jis_to_utf8(ik_name);
		return true;
	}

	bool VMDIkController::write_to_file(const file& file) const
	{
		const std::string ik_name = code_converter::utf8_to_shift_jis(m_data->IK_name);
		if (!file.write_elements(ik_name.data(), 20LLU))
			return false;
		if (!file.write_elements(m_data->IK_enable))
			return false;
		return true;
	}
}
