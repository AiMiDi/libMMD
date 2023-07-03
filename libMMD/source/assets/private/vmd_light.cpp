/**************************************************************************

Copyright:Copyright(c) 2022-present, Aimidi & Walter White & CMT contributors.
Author:			Aimidi
Date:			2022/7/15
File:			vmd_light_animation.cpp
Description:	MMD style lighting animation

**************************************************************************/

#include "vmd_light.h"

namespace libmmd
{
	bool VMDLightAnimation::read_from_file(const file& file)
	{
		if (!file.read_elements(frame_num_))
			return false;
		if (!file.read_elements(light_color_))
			return false;
		if (!file.read_elements(light_position_))
			return false;
		return true;
	}

	bool VMDLightAnimation::write_to_file(const file& file) const
	{
		if (!file.write_elements(frame_num_))
			return false;
		if (!file.write_elements(light_color_))
			return false;
		if (!file.write_elements(light_position_))
			return false;
		return true;
	}
}
