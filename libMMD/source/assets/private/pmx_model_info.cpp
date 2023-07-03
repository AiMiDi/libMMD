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
	void pmx_model_description::reset_reader()
	{
		read_text.set_text_encoding(text_encoding_);
		read_vertex_index.set_index_size(vertex_index_size_);
		read_texture_index.set_index_size(texture_index_size_);
		read_material_index.set_index_size(material_index_size_);
		read_bone_index.set_index_size(bone_index_size_);
		read_morph_index.set_index_size(morph_index_size_);
		read_rigid_body_index.set_index_size(rigid_body_index_size_);
	}

	bool pmx_model_description::read_from_file(const file& file)
	{
		if (!file.read_elements(model_version_))
			return false;
		Char globals_count;
		if (!file.read_elements(globals_count))
			return false;
		if (globals_count != 8)
		{
			ELOGE << "pmx_model_description::LoadFromFile() error at read global info count";
			return false;
		}
		if (!file.read_elements(text_encoding_))
			return false;
		if (!file.read_elements(additional_vec4_count_))
			return false;
		if (!file.read_elements(vertex_index_size_))
			return false;
		if (!file.read_elements(texture_index_size_))
			return false;
		if (!file.read_elements(material_index_size_))
			return false;
		if (!file.read_elements(bone_index_size_))
			return false;
		if (!file.read_elements(morph_index_size_))
			return false;
		if (!file.read_elements(rigid_body_index_size_))
			return false;
		reset_reader();
		if (!read_text(file, model_name_local_))
			return false;
		if (!read_text(file, model_name_universal_))
			return false;
		if (!read_text(file, comments_local_))
			return false;
		if (!read_text(file, comments_universal_))
			return false;
		return true;
	}
}
