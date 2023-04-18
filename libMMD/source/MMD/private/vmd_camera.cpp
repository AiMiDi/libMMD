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
		if (!file.read_elements(m_data->m_distance))
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
		if (!m_data->m_interpolator_distance.read(file))
			return false;
		if (!m_data->m_interpolator_AOV.read(file))
			return false;
		if (!file.read_elements(m_data->m_viewing_angle))
			return false;
		if (!file.read_elements(m_data->m_perspective))
			return false;
		return true;
	}

	bool VMDCameraAnimation::write_to_file(const file& file) const
	{
		if (!file.write_elements(frame_num_))
			return false;
		if (!file.write_elements(m_data->m_distance))
			return false;
		if (!file.write_elements(m_data->m_position))
			return false;
		if (!file.write_elements(m_data->m_rotation))
			return false;
		if (!m_data->m_interpolator_position_x.write(file))
			return false;
		if (!m_data->m_interpolator_position_y.write(file))
			return false;
		if (!m_data->m_interpolator_position_z.write(file))
			return false;
		if (!m_data->m_interpolator_rotation.write(file))
			return false;
		if (!m_data->m_interpolator_distance.write(file))
			return false;
		if (!m_data->m_interpolator_AOV.write(file))
			return false;
		if (!file.write_elements(m_data->m_viewing_angle))
			return false;
		if (!file.write_elements(m_data->m_perspective))
			return false;
		return true;
	}
}
