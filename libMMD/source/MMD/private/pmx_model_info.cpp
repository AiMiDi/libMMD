/**************************************************************************

Copyright:Copyright(c) 2022-present, Aimidi & Walter White & CMT contributors.
Author:			Aimidi
Date:			2022/8/14
File:			pmx_model_info.cpp
Description:	pmx model information data

**************************************************************************/

#include "pmx_model_info.h"

namespace libmmd
{
	void PMXModelInfo::reset_reader()
	{
		m_text_reader.set_text_encoding(m_data->text_encoding);
		m_vertex_index_reader.set_index_size(m_data->vertex_index_size);
		m_texture_index_reader.set_index_size(m_data->texture_index_size);
		m_material_index_reader.set_index_size(m_data->material_index_size);
		m_bone_index_reader.set_index_size(m_data->bone_index_size);
		m_morph_index_reader.set_index_size(m_data->morph_index_size);
		m_rigidbody_index_reader.set_index_size(m_data->rigidbody_index_size);
	}

	bool PMXModelInfo::read_from_file(const file& file)
	{
		if (!file.read_elements(m_data->version))
			return false;
		Char globals_count;
		if (!file.read_elements(globals_count))
			return false;
		if (globals_count != 8)
		{
			ApplicationOutput("PMXModelInfo::LoadFromFile() error at read global info count");
			return false;
		}
		if (!file.read_elements(m_data->text_encoding))
			return false;
		if (!file.read_elements(m_data->additional_vec4_count))
			return false;
		if (!file.read_elements(m_data->vertex_index_size))
			return false;
		if (!file.read_elements(m_data->texture_index_size))
			return false;
		if (!file.read_elements(m_data->material_index_size))
			return false;
		if (!file.read_elements(m_data->bone_index_size))
			return false;
		if (!file.read_elements(m_data->morph_index_size))
			return false;
		if (!file.read_elements(m_data->rigidbody_index_size))
			return false;
		reset_reader();
		if (!m_text_reader(file, m_data->model_name_local))
			return false;
		if (!m_text_reader(file, m_data->model_name_universal))
			return false;
		if (!m_text_reader(file, m_data->comments_local))
			return false;
		if (!m_text_reader(file, m_data->comments_universal))
			return false;
		return true;
	}
}
