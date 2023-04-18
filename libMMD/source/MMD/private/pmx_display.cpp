/**************************************************************************

Copyright:Copyright(c) 2022-present, Aimidi & Walter White & CMT contributors.
Author:			Aimidi
Date:			2022/9/19
File:			pmx_display.cpp
Description:	pmx display data

**************************************************************************/

#include "pmx_display.h"

namespace libmmd
{
	bool PMXDisplay::read_from_file(const file& file)
	{
		if (!model_info_->m_text_reader(file, m_data->display_name_local))
			return false;
		if (!model_info_->m_text_reader(file, m_data->display_name_universal))
			return false;
		if (!file.read_elements(m_data->special_flag))
			return false;
		if (!file.read_elements(m_data->frame_count))
			return false;

		for (Int32 frame_index = 0; frame_index < m_data->frame_count; frame_index++)
		{
			auto& [frame_type, frame_data] = m_data->frame_datas[frame_index];
			if (!file.read_elements(frame_type))
				return false;
			switch (frame_type)
			{
			case PMXFrameData::BONE:
			{
				model_info_->m_bone_index_reader(file, frame_data);
				break;
			}
			case PMXFrameData::MORPH:
			{
				model_info_->m_morph_index_reader(file, frame_data);
				break;
			}
			default:
				return false;
			}
		}
		return true;
	}
}