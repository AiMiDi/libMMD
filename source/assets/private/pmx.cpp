/**************************************************************************

Copyright:Copyright(c) 2022-present, Aimidi & Walter White & CMT contributors.
Author:			Aimidi
Date:			2022/7/2
File:			pmx_data.cpp
Description:	pmx file data

**************************************************************************/

#include "pmx.h"

namespace libmmd
{

	bool pmx_model_impl::read_from_file_impl(const path& path)
	{
		file file;
		if (!path.check_suffix("pmx"))
			return false;

		if (!file.open(path, file::open_mode::READ))
			return false;

		// read signature, value is "PMX "
		if (char signature[5]{ 0 }; !file.read_elements(signature, 4) && strncmp(signature, "PMX", 3) != 0)
		{
			ELOGE << "PMX::read_from_file(): error at read signature.";
			return false;
		}

		// read model info
		if (!model_description_.read_from_file(file))
		{
			ELOGE << "PMX::read_from_file(): error at read model info";
			return false;
		}

		// read vertex data
		vertex_data_.read_from_file(file);

		// read surface data
		surface_data_.read_from_file(file);

		// read texture data
		texture_data_.read_from_file(file);

		// read material data
		material_data_.read_from_file(file);

		// read bone data
		bone_data_.read_from_file(file);

		// read morph data
		morph_data_.read_from_file(file);

		// read rigid body data
		rigid_body_data_.read_from_file(file);

		// read joint data
		joint_data_.read_from_file(file);

		return true;
	}

	bool pmx_model_impl::write_to_file_impl(const path& path) const
	{
		// TODO: write_to_file
		return false;
	}

	pmx_model::pmx_version pmx_model_impl::get_version() const
	{
		if (std::fabs(model_description_.get_model_version() - 2.1f) < std::numeric_limits<float>::epsilon())
			return pmx_version::V2_1;
		return pmx_version::V2_0;
	}

	void pmx_model_impl::set_version(pmx_version version)
	{
		switch (version) {
		case pmx_version::V2_0:
			{
				model_description_.set_model_version(2.0f);
				break;
			}
		case pmx_version::V2_1: 
			{
				model_description_.set_model_version(2.1f);
				break;
			}
		}
	}

	inline std::string pmx_model_impl::get_model_name_local() const
	{
		auto& name = model_description_.get_model_name_local();
		return std::string{name.begin(), name.end()};
	}

	inline void pmx_model_impl::set_model_name_local(const std::string& name)
	{
		model_description_.set_model_name_local(std::u8string(name.begin(), name.end()));
	}

	inline std::string pmx_model_impl::get_model_name_universal() const
	{
		auto& name = model_description_.get_model_name_universal();
		return std::string{name.begin(), name.end()};
	}

	inline void pmx_model_impl::set_model_name_universal(const std::string& name)
	{
		model_description_.set_model_name_universal(std::u8string(name.begin(), name.end()));
	}

	inline std::string pmx_model_impl::get_comments_local() const
	{
		auto& comment = model_description_.get_comments_local();
		return std::string{comment.begin(), comment.end()};
	}

	inline void pmx_model_impl::set_comments_local(const std::string& comments)
	{
		model_description_.set_comments_local(std::u8string(comments.begin(), comments.end()));
	}

	inline std::string pmx_model_impl::get_comments_universal() const
	{
		auto& comment = model_description_.get_comments_universal();
		return std::string{comment.begin(), comment.end()};
	}

	inline void pmx_model_impl::set_comments_universal(const std::string& comments)
	{
		model_description_.set_model_name_universal(std::u8string(comments.begin(), comments.end()));
	}

	inline const pmx_model::pmx_vertex_array& pmx_model_impl::get_pmx_vertex_array()
	{
		return vertex_data_;
	}

	inline pmx_model::pmx_vertex_array& pmx_model_impl::mutable_pmx_vertex_array()
	{
		return vertex_data_;
	}

	inline const pmx_model::pmx_surface_array& pmx_model_impl::get_pmx_surface_array()
	{
		return surface_data_;
	}

	inline pmx_model::pmx_surface_array& pmx_model_impl::mutable_pmx_surface_array()
	{
		return surface_data_;
	}

	inline const pmx_model::pmx_texture_array& pmx_model_impl::get_pmx_texture_array()
	{
		return texture_data_;
	}

	inline pmx_model::pmx_texture_array& pmx_model_impl::mutable_pmx_texture_array()
	{
		return texture_data_;
	}

	inline const pmx_model::pmx_material_array& pmx_model_impl::get_pmx_material_array()
	{
		return material_data_;
	}

	inline pmx_model::pmx_material_array& pmx_model_impl::mutable_pmx_material_array()
	{
		return material_data_;
	}

	inline const pmx_model::pmx_bone_array& pmx_model_impl::get_pmx_bone_array()
	{
		return bone_data_;
	}

	inline pmx_model::pmx_bone_array& pmx_model_impl::mutable_pmx_bone_array()
	{
		return bone_data_;
	}

	inline const pmx_model::pmx_morph_array& pmx_model_impl::get_pmx_morph_array()
	{
		return morph_data_;
	}

	inline pmx_model::pmx_morph_array& pmx_model_impl::mutable_pmx_morph_array()
	{
		return morph_data_;
	}

	inline const pmx_model::pmx_display_frame_array& pmx_model_impl::get_pmx_display_frame_array()
	{
		return display_data_;
	}

	inline pmx_model::pmx_display_frame_array& pmx_model_impl::mutable_pmx_display_frame_array()
	{
		return display_data_;
	}

	inline const pmx_model::pmx_rigid_body_array& pmx_model_impl::get_pmx_rigid_body_array()
	{
		return rigid_body_data_;
	}

	inline pmx_model::pmx_rigid_body_array& pmx_model_impl::mutable_pmx_rigid_body_array()
	{
		return rigid_body_data_;
	}

	inline const pmx_model::pmx_joint_array& pmx_model_impl::get_pmx_joint_array()
	{
		return joint_data_;
	}

	inline pmx_model::pmx_joint_array& pmx_model_impl::mutable_pmx_joint_array()
	{
		return joint_data_;
	}

	inline bool pmx_model_impl::read_from_file(const std::string& file_name)
	{
		const path path{ file_name };
		return read_from_file_impl(path);
	}

	inline bool pmx_model_impl::write_to_file(const std::string& file_name) const
	{
		const path path{ file_name };
		return write_to_file_impl(path);
	}

	inline bool pmx_model_impl::read_from_file(const std::wstring& file_name)
	{
		const path path{ file_name };
		return read_from_file_impl(path);
	}

	inline bool pmx_model_impl::write_to_file(const std::wstring& file_name) const
	{
		const path path{ file_name };
		return write_to_file_impl(path);
	}
}
