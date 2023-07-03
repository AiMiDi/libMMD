/**************************************************************************

Copyright:Copyright(c) 2022-present, Aimidi & Walter White & CMT contributors.
Author:			Aimidi
Date:			2022/7/16
File:			vmd_camera_animation.cpp
Description:	

**************************************************************************/

#include "vmd_camera.h"

namespace libmmd
{
	bool VMDCameraAnimation::read_from_file(const file& file)
	{
		if (!file.read_elements(frame_num_))
			return false;
		if (!file.read_elements(distance_))
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
		if (!interpolator_distance_.read(file))
			return false;
		if (!interpolator_viewing_angle_.read(file))
			return false;
		if (!file.read_elements(viewing_angle_))
			return false;
		if (!file.read_elements(perspective_))
			return false;
		return true;
	}

	bool VMDCameraAnimation::write_to_file(const file& file) const
	{
		if (!file.write_elements(frame_num_))
			return false;
		if (!file.write_elements(distance_))
			return false;
		if (!file.write_elements(position_))
			return false;
		if (!file.write_elements(rotation_))
			return false;
		if (!interpolator_position_x_.write(file))
			return false;
		if (!interpolator_position_y_.write(file))
			return false;
		if (!interpolator_position_z_.write(file))
			return false;
		if (!interpolator_rotation_.write(file))
			return false;
		if (!interpolator_distance_.write(file))
			return false;
		if (!interpolator_viewing_angle_.write(file))
			return false;
		if (!file.write_elements(viewing_angle_))
			return false;
		if (!file.write_elements(perspective_))
			return false;
		return true;
	}
}
