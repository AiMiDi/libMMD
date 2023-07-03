/**************************************************************************

Copyright:Copyright(c) 2022-present, Aimidi & Walter White & CMT contributors.
Author:			Aimidi
Date:			2022/7/14
File:			vmd_model_controller_animation.cpp
Description:	MMD style model information animation

**************************************************************************/

#include "vmd_model_controller.h"

namespace libmmd
{
	bool VMDModelController::read_from_file(const file& file)
	{
		if (!file.read_elements(frame_num_))
			return false;
		if (!file.read_elements(show_))
			return false;
		UInt32 ik_info_number = 0;
		if (!file.read_elements(ik_info_number))
			return false;
		IK_controller_animation_.read_from_file(file);
		return true;
	}

	bool VMDModelController::write_to_file(const file& file) const
	{
		if (!file.write_elements(frame_num_))
			return false;
		if (!file.write_elements(show_))
			return false;
		if (!IK_controller_animation_.write_to_file(file))
			return false;
		return true;
	}
}
