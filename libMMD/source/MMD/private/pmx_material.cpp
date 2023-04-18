/**************************************************************************

Copyright:Copyright(c) 2022-present, Aimidi & Walter White & CMT contributors.
Author:			Aimidi
Date:			2022/9/14
File:			pmx_material.cpp
Description:	pmx material data

**************************************************************************/

#include "pmx_material.h"

namespace libmmd
{
	bool PMXMaterial::read_from_file(const file& file)
	{
		if (!model_info_->m_text_reader(file, m_data->material_name_local))
			return false;
		if (!model_info_->m_text_reader(file, m_data->material_name_universal))
			return false;
		if (!file.read_elements(m_data->diffuse_color))
			return false;
		if (!file.read_elements(m_data->specular_color))
			return false;
		if (!file.read_elements(m_data->specular_strength))
			return false;
		if (!file.read_elements(m_data->ambient_color))
			return false;
		if (!file.read_elements(m_data->drawing_flags))
			return false;
		if (!file.read_elements(m_data->edge_color))
			return false;
		if (!file.read_elements(m_data->edge_scale))
			return false;
		if (!model_info_->m_texture_index_reader(file, m_data->texture_index))
			return false;
		if (!model_info_->m_texture_index_reader(file, m_data->environment_index))
			return false;
		if (!file.read_elements(m_data->environment_blend_mode))
			return false;
		if (!file.read_elements(m_data->toon_reference))
			return false;
		if (m_data->toon_reference == 0)
		{
			if (!model_info_->m_texture_index_reader(file, m_data->toon_part))
				return false;
		}
		else if (m_data->toon_reference == 1)
		{
			if (!file.read_elements(m_data->toon_internal))
				return false;
		}
		if (!model_info_->m_text_reader(file, m_data->meta_data))
			return false;
		if (!file.read_elements(m_data->surface_count))
			return false;
		m_data->surface_count /= 3;
		return true;
	}
}