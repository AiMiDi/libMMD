/**************************************************************************

Copyright:Copyright(c) 2022-present, Aimidi & Walter White & CMT contributors.
Author:			Aimidi
Date:			2022/9/24
File:			pmx_rigid_body.cpp
Description:	pmx rigid body data

**************************************************************************/

#include "pmx_rigid_body.h"

namespace libmmd
{
	bool PMXRigidBody::read_from_file(const file& file)
	{
		if (!model_info_->m_text_reader(file, m_data->rigid_body_name_local))
			return false;
		if (!model_info_->m_text_reader(file, m_data->rigid_body_name_universal))
			return false;
		if (!model_info_->m_bone_index_reader(file, m_data->related_bone_index))
			return false;
		if (!file.read_elements(m_data->group_id))
			return false;
		if (!file.read_elements(m_data->non_collision_group))
			return false;
		if (!file.read_elements(m_data->shape_type))
			return false;
		if (!file.read_elements(m_data->shape_size))
			return false;
		if (!file.read_elements(m_data->shape_position))
			return false;
		if (!file.read_elements(m_data->shape_rotation))
			return false;
		if (!file.read_elements(m_data->mass))
			return false;
		if (!file.read_elements(m_data->move_attenuation))
			return false;
		if (!file.read_elements(m_data->rotation_damping))
			return false;
		if (!file.read_elements(m_data->repulsion_force))
			return false;
		if (!file.read_elements(m_data->friction_force))
			return false;
		if (!file.read_elements(m_data->physics_mode))
			return false;
		return true;
	}
}