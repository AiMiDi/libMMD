/**************************************************************************

Copyright:Copyright(c) 2022-present, Aimidi & Walter White & CMT contributors.
Author:			Aimidi
Date:			2022/9/24
File:			pmx_joint.cpp
Description:	pmx joint data

**************************************************************************/

#include "pmx_joint.h"

namespace libmmd
{
	bool PMXJoint::read_from_file(const file& file)
	{
		if (!model_info_->m_text_reader(file, m_data->joint_name_local))
			return false;
		if (!model_info_->m_text_reader(file, m_data->joint_name_universal))
			return false;
		if (!file.read_elements(m_data->joint_type))
			return false;
		if (!model_info_->m_rigidbody_index_reader(file, m_data->rigid_body_index_A))
			return false;
		if (!model_info_->m_rigidbody_index_reader(file, m_data->rigid_body_index_B))
			return false;
		if (!file.read_elements(m_data->position))
			return false;
		if (!file.read_elements(m_data->rotation))
			return false;
		if (!file.read_elements(m_data->position_minimum))
			return false;
		if (!file.read_elements(m_data->position_maximum))
			return false;
		if (!file.read_elements(m_data->rotation_minimum))
			return false;
		if (!file.read_elements(m_data->rotation_maximum))
			return false;
		if (!file.read_elements(m_data->position_spring))
			return false;
		if (!file.read_elements(m_data->rotation_spring))
			return false;
		return true;
	}
}