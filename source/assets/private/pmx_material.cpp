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
	 std::string pmx_material_impl::get_material_name_local() const
	{
		return std::string{material_name_local_.begin(), material_name_local_.end()};
	}

	 void pmx_material_impl::set_material_name_local(const std::string& name)
	{
		material_name_local_ = std::u8string{ name.begin(),name.end() };
	}

	 std::string pmx_material_impl::get_material_name_universal() const
	{
		return std::string{material_name_universal_.begin(), material_name_universal_.end()};
	}

	 void pmx_material_impl::set_material_name_universal(const std::string& name)
	{
		material_name_universal_ = std::u8string{ name.begin(),name.end() };
	}

	 const std::array<float, 4>& pmx_material_impl::get_diffuse_color() const
	{
		return diffuse_color_;
	}

	 void pmx_material_impl::set_diffuse_color(const std::array<float, 4>& color)
	{
		diffuse_color_ = color;
	}

	 const std::array<float, 3>& pmx_material_impl::get_specular_color() const
	{
		return specular_color_;
	}

	 void pmx_material_impl::set_specular_color(const std::array<float, 3>& color)
	{
		specular_color_ = color;
	}

	 float pmx_material_impl::get_specular_strength() const
	{
		return specular_strength_;
	}

	 void pmx_material_impl::set_specular_strength(const float strength)
	{
		specular_strength_ = strength;
	}

	 const std::array<float, 3>& pmx_material_impl::get_ambient_color() const
	{
		return ambient_color_;
	}

	 void pmx_material_impl::set_ambient_color(const std::array<float, 3>& color)
	{
		ambient_color_ = color;
	}

	 bool pmx_material_impl::is_double_side() const
	{
		return material_flags_.double_side;
	}

	 void pmx_material_impl::set_is_double_side(const bool value)
	{
		material_flags_.double_side = value;
	}

	 bool pmx_material_impl::have_ground_shadow() const
	{
		return material_flags_.ground_shadow;
	}

	 void pmx_material_impl::set_have_ground_shadow(const bool value)
	{
		material_flags_.ground_shadow = value;
	}

	 bool pmx_material_impl::is_draw_shadow() const
	{
		return material_flags_.draw_shadow;
	}

	 void pmx_material_impl::set_is_draw_shadow(const bool value)
	{
		material_flags_.draw_shadow = value;
	}

	 bool pmx_material_impl::is_receive_shadow() const
	{
		return material_flags_.receive_shadow;
	}

	 void pmx_material_impl::set_is_receive_shadow(const bool value)
	{
		material_flags_.receive_shadow = value;
	}

	 bool pmx_material_impl::is_draw_edge() const
	{
		return material_flags_.draw_edge;
	}

	 void pmx_material_impl::set_have_edge(const bool value)
	{
		material_flags_.draw_edge = value;
	}

	 bool pmx_material_impl::is_vertex_color() const
	{
		return material_flags_.vertex_color;
	}

	 void pmx_material_impl::set_is_vertex_color(const bool value)
	{
		material_flags_.vertex_color = value;
	}

	 bool pmx_material_impl::is_point_drawing() const
	{
		return material_flags_.point_drawing;
	}

	 void pmx_material_impl::set_is_point_drawing(const bool value)
	{
		material_flags_.point_drawing = value;
	}

	 bool pmx_material_impl::is_line_drawing() const
	{
		return material_flags_.line_drawing;
	}

	 void pmx_material_impl::set_is_line_drawing(const bool value)
	{
		material_flags_.line_drawing = value;
	}

	 const std::array<float, 4>& pmx_material_impl::get_edge_color() const
	{
		return edge_color_;
	}

	 void pmx_material_impl::set_edge_color(const std::array<float, 4>& color)
	{
		edge_color_ = color;
	}

	 float pmx_material_impl::get_edge_scale() const
	{
		return edge_scale_;
	}

	 void pmx_material_impl::set_edge_scale(const float scale)
	{
		edge_scale_ = scale;
	}

	 int32_t pmx_material_impl::get_texture_index() const
	{
		return texture_index_;
	}

	 void pmx_material_impl::set_texture_index(const int32_t index)
	{
		texture_index_ = index;
	}

	 pmx_material::environment_blend_mode pmx_material_impl::get_environment_blend_mode() const
	{
		return environment_blend_mode_;
	}

	 void pmx_material_impl::set_environment_blend_mode(const environment_blend_mode mode)
	{
		environment_blend_mode_ = mode;
	}

	 int32_t pmx_material_impl::get_environment_index() const
	{
		return environment_index_;
	}

	 void pmx_material_impl::set_environment_index(const int32_t index)
	{
		environment_index_ = index;
	}

	 pmx_material::toon_reference_type pmx_material_impl::get_toon_reference() const
	{
		return toon_reference_;
	}

	 void pmx_material_impl::set_toon_reference(const toon_reference_type reference)
	{
		toon_reference_ = reference;
	}

	 int32_t pmx_material_impl::get_toon_part() const
	{
		return toon_part_;
	}

	 void pmx_material_impl::set_toon_part(const int32_t part)
	{
		toon_part_ = part;
	}

	 int8_t pmx_material_impl::get_toon_internal() const
	{
		return toon_internal_;
	}

	 void pmx_material_impl::set_toon_internal(const int8_t internal)
	{
		toon_internal_ = internal;
	}

	 std::string pmx_material_impl::get_meta_data() const
	{
		return std::string{meta_data_.begin(), meta_data_.end()};
	}

	 void pmx_material_impl::set_meta_data(const std::string& data)
	{
		meta_data_ = std::u8string{data.begin(), data.end()};
	}

	 int32_t pmx_material_impl::get_surface_count() const
	{
		return surface_count_;
	}

	 void pmx_material_impl::set_surface_count(const int32_t count)
	{
		surface_count_ = count;
	}

	bool pmx_material_impl::read_from_file(const file& file)
	{
		if (!model_description_->read_text(file, material_name_local_))
			return false;
		if (!model_description_->read_text(file, material_name_universal_))
			return false;
		if (!file.read_elements(diffuse_color_))
			return false;
		if (!file.read_elements(specular_color_))
			return false;
		if (!file.read_elements(specular_strength_))
			return false;
		if (!file.read_elements(ambient_color_))
			return false;
		if (!file.read_elements(material_flags_))
			return false;
		if (!file.read_elements(edge_color_))
			return false;
		if (!file.read_elements(edge_scale_))
			return false;
		if (!model_description_->read_texture_index(file, texture_index_))
			return false;
		if (!model_description_->read_texture_index(file, environment_index_))
			return false;
		if (!file.read_elements(environment_blend_mode_))
			return false;
		if (!file.read_elements(toon_reference_))
			return false;
		if (toon_reference_ == toon_reference_type::TEXTURE)
		{
			if (!model_description_->read_texture_index(file, toon_part_))
				return false;
		}
		else if (toon_reference_ == toon_reference_type::INTERNAL)
		{
			if (!file.read_elements(toon_internal_))
				return false;
		}
		if (!model_description_->read_text(file, meta_data_))
			return false;
		if (!file.read_elements(surface_count_))
			return false;
		surface_count_ /= 3;
		return true;
	}

	bool pmx_material_impl::write_to_file(const file& file) const
	{
		// TODO: write to file
		return false;
	}
}
