/**************************************************************************

Copyright:Copyright(c) 2023-present, Aimidi & libmmd contributors.
Author:			Aimidi
Date:			2023/7/5
File:			pmx_model_description.cpp
Description:	pmx model description data

**************************************************************************/

#include "pmx_model_description.h"

namespace libmmd
{
	float pmx_model_description::get_model_version() const
	{
		return model_version_;
	}

	void pmx_model_description::set_model_version(float version)
	{
		model_version_ = version;
	}

	const std::u8string& pmx_model_description::get_model_name_local() const
	{
		return  model_name_local_;
	}

	void pmx_model_description::set_model_name_local(const std::u8string& name)
	{
		model_name_local_ = name;
	}

	const std::u8string& pmx_model_description::get_model_name_universal() const
	{
		return  model_name_universal_;
	}

	void pmx_model_description::set_model_name_universal(const std::u8string& name)
	{
		model_name_universal_ = name;
	}

	const std::u8string& pmx_model_description::get_comments_local() const
	{
		return  comments_local_;
	}

	void pmx_model_description::set_comments_local(const std::u8string& comments)
	{
		comments_local_ = comments;
	}

	const std::u8string& pmx_model_description::get_comments_universal() const
	{
		return  comments_universal_;
	}

	void pmx_model_description::set_comments_universal(const std::u8string& comments)
	{
		comments_universal_ = comments;
	}

	int8_t pmx_model_description::get_additional_vec4_count() const
	{
		return additional_vec4_count_;
	}

	void pmx_model_description::set_additional_vec4_count(int8_t count)
	{
		additional_vec4_count_ = count;
	}

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
		if (!file.read_element(model_version_))
			return false;
		Char globals_count;
		if (!file.read_element(globals_count))
			return false;
		if (globals_count != 8)
		{
			ELOGE << "pmx_model_description::LoadFromFile() error at read global info count";
			return false;
		}
		if (!file.read_element(text_encoding_))
			return false;
		if (!file.read_element(additional_vec4_count_))
			return false;
		if (!file.read_element(vertex_index_size_))
			return false;
		if (!file.read_element(texture_index_size_))
			return false;
		if (!file.read_element(material_index_size_))
			return false;
		if (!file.read_element(bone_index_size_))
			return false;
		if (!file.read_element(morph_index_size_))
			return false;
		if (!file.read_element(rigid_body_index_size_))
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
