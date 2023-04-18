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
		if (!file.read_elements(m_data->show))
			return false;
		UInt32 ik_info_number = 0;
		if (!file.read_elements(ik_info_number))
			return false;
		m_data->IK_Info_array.resize(ik_info_number);
		for (UInt32 ik_info_index = 0; ik_info_index < ik_info_number; ++ik_info_index)
		{
			if (!m_data->IK_Info_array[ik_info_index].read_from_file(file))
				return false;
		}
		return true;
	}

	bool VMDModelController::write_to_file(const file& file) const
	{
		if (!file.write_elements(frame_num_))
			return false;
		if (!file.write_elements(m_data->show))
			return false;
		auto IK_info_count = static_cast<UInt32>(m_data->IK_Info_array.size());
		if (!file.write_elements(IK_info_count))
			return false;
		while (IK_info_count--)
		{
			if (!m_data->IK_Info_array[IK_info_count].write_to_file(file))
				return false;
		}
		return true;
	}
}
