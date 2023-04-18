/**************************************************************************

Copyright:Copyright(c) 2022-present, Aimidi & Walter White & CMT contributors.
Author:			Aimidi
Date:			2022/8/24
File:			pmx_bone.cpp
Description:	pmx bone data

**************************************************************************/

#include "pmx_bone.h"

namespace libmmd
{
	bool PMXBone::read_from_file(const file& file)
	{
		if (!model_info_->m_text_reader(file, m_data->bone_name_local))
			return false;
		if (!model_info_->m_text_reader(file, m_data->bone_name_universal))
			return false;
		if (!file.read_elements(m_data->position))
			return false;
		if (!model_info_->m_bone_index_reader(file, m_data->parent_bone_index))
			return false;
		if (!file.read_elements(m_data->layer))
			return false;
		if (!file.read_elements(m_data->bone_flags))
			return false;
		if (m_data->bone_flags.indexed_tail_position == 0)
		{
			if (!file.read_elements(m_data->tail_position))
				return false;
		}
		else if (m_data->bone_flags.indexed_tail_position == 1)
		{
			if (!model_info_->m_bone_index_reader(file, m_data->tail_index))
				return false;
		}
		if (m_data->bone_flags.inherit_rotation || m_data->bone_flags.inherit_translation)
		{
			if (!model_info_->m_bone_index_reader(file, m_data->inherit_bone_parent_index))
				return false;
			if (!file.read_elements(m_data->inherit_bone_parent_influence))
				return false;
		}
		if (m_data->bone_flags.fixed_axis)
		{
			if (!file.read_elements(m_data->bone_fixed_axis))
				return false;
		}
		if (m_data->bone_flags.local_coordinate)
		{
			if (!file.read_elements(m_data->bone_local_X))
				return false;
			if (!file.read_elements(m_data->bone_local_Z))
				return false;
		}
		if (m_data->bone_flags.external_parent_deform)
		{
			if (!file.seek(4))
				return false;
		}
		if (m_data->bone_flags.is_IK)
		{
			if (!model_info_->m_bone_index_reader(file, m_data->IK_target_index))
				return false;
			if (!file.read_elements(m_data->IK_loop_count))
				return false;
			if (!file.read_elements(m_data->IK_limit_radian))
				return false;
			if (!file.read_elements(m_data->IK_link_count))
				return false;
			m_data->IK_links.resize(m_data->IK_link_count);
			for (Int32 j = 0; j < m_data->IK_link_count; j++)
			{
				auto& [bone_index, has_limits, limit_min, limit_max] = m_data->IK_links[j];
				if (!model_info_->m_bone_index_reader(file, bone_index))
					return false;
				if (!file.read_elements(has_limits))
					return false;
				if (has_limits)
				{
					if (!file.read_elements(limit_min))
						return false;
					if (!file.read_elements(limit_max))
						return false;
				}
			}
		}
		return true;
	}
}