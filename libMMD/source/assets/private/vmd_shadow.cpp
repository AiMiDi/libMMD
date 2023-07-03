/**************************************************************************

Copyright:Copyright(c) 2022-present, Aimidi & Walter White & CMT contributors.
Author:			Aimidi
Date:			2022/7/15
File:			vmd_shadow_animation.cpp
Description:	MMD style shadow animation

**************************************************************************/

#include "vmd_shadow.h"

namespace libmmd
{
	bool VMDShadow::read_from_file(const file& file)
	{
		if (!file.read_elements(frame_num_))
			return false;
		if (!file.read_elements(shadow_type_))
			return false;
		if (!file.read_elements(distance_))
			return false;
		return true;
	}

	bool VMDShadow::write_to_file(const file& file) const
	{
		if (!file.write_elements(frame_num_))
			return false;
		if (!file.write_elements(shadow_type_))
			return false;
		if (!file.write_elements(distance_))
			return false;
		return true;
	}
}