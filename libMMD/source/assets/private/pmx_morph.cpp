/**************************************************************************

Copyright:Copyright(c) 2022-present, Aimidi & Walter White & CMT contributors.
Author:			Aimidi
Date:			2022/9/18
File:			pmx_morph.cpp
Description:	pmx morph data

**************************************************************************/

#include "pmx_morph.h"

namespace libmmd
{
	inline int32_t pmx_group_morph_offset_impl::get_morph_index() const
	{
		return  morph_index_;
	}

	inline void pmx_group_morph_offset_impl::set_morph_index(const int32_t morph_index)
	{
		morph_index_ = morph_index;
	}

	inline float pmx_group_morph_offset_impl::get_morph_weight() const
	{
		return morph_weight_;
	}

	inline void pmx_group_morph_offset_impl::set_morph_weight(const float morph_weight)
	{
		morph_weight_ = morph_weight;
	}

	bool pmx_group_morph_offset_impl::read_from_file(const file& file)
	{
		if (!model_description_->read_morph_index(file, morph_index_))
			return false;
		if (!file.read_elements(morph_weight_))
			return false;
		return true;
	}

	bool pmx_group_morph_offset_impl::write_to_file(const file& file) const
	{
		// TODO: write to file
		return false;
	}

	inline uint32_t pmx_vertex_morph_offset_impl::get_vertex_index() const
	{
		return vertex_index_;
	}

	inline void pmx_vertex_morph_offset_impl::set_vertex_index(const uint32_t vertex_index)
	{
		vertex_index_ = vertex_index;
	}

	inline const std::array<float, 3>& pmx_vertex_morph_offset_impl::get_offset_position() const
	{
		return offset_position_;
	}

	inline void pmx_vertex_morph_offset_impl::set_offset_position(const std::array<float, 3>& offset_position)
	{
		offset_position_ = offset_position;
	}

	bool pmx_vertex_morph_offset_impl::read_from_file(const file& file)
	{
		if (!model_description_->read_vertex_index(file, vertex_index_))
			return false;
		if (!file.read_elements(offset_position_))
			return false;
		return true;
	}

	bool pmx_vertex_morph_offset_impl::write_to_file(const file& file) const
	{
		// TODO: write to file
		return false;
	}

	inline int32_t pmx_bone_morph_offset_impl::get_bone_index() const
	{
		return bone_index_;
	}

	inline void pmx_bone_morph_offset_impl::set_bone_index(const int32_t bone_index)
	{
		bone_index_ = bone_index;
	}

	inline const std::array<float, 3>& pmx_bone_morph_offset_impl::get_bone_translation() const
	{
		return bone_translation_;
	}

	inline void pmx_bone_morph_offset_impl::set_bone_translation(const std::array<float, 3>& bone_translation)
	{
		bone_translation_ = bone_translation;
	}

	inline const std::array<float, 3>& pmx_bone_morph_offset_impl::get_bone_rotation() const
	{
		return bone_rotation_;
	}

	inline void pmx_bone_morph_offset_impl::set_bone_rotation(const std::array<float, 3>& bone_rotation)
	{
		bone_rotation_ = bone_rotation;
	}

	bool pmx_bone_morph_offset_impl::read_from_file(const file& file)
	{
		if (!model_description_->read_bone_index(file, bone_index_))
			return false;
		if (!file.read_elements(bone_translation_))
			return false;
		Vector4d32 quaternion_rotation;
		if (!file.read_elements(quaternion_rotation))
			return false;
		bone_rotation_ = quaternion_to_euler(quaternion_rotation);
		return true;
	}

	bool pmx_bone_morph_offset_impl::write_to_file(const file& file) const
	{
		// TODO: write to file
		return false;
	}

	inline uint32_t pmx_uv_morph_offset_impl::get_vertex_index() const
	{
		return vertex_index_;
	}

	inline void pmx_uv_morph_offset_impl::set_vertex_index(uint32_t vertex_index)
	{
		vertex_index_ = vertex_index;
	}

	inline const std::array<float, 4>& pmx_uv_morph_offset_impl::get_uv_offset() const
	{
		return uv_offset_;
	}

	inline void pmx_uv_morph_offset_impl::set_uv_offset(const std::array<float, 4>& offset)
	{
		uv_offset_ = offset;
	}

	bool pmx_uv_morph_offset_impl::read_from_file(const file& file)
	{
		if (!model_description_->read_vertex_index(file, vertex_index_))
			return false;
		if (!file.read_elements(uv_offset_))
			return false;
		return true;
	}

	bool pmx_uv_morph_offset_impl::write_to_file(const file& file) const
	{
		// TODO: write to file
		return false;
	}

	inline int32_t pmx_material_morph_offset_impl::get_material_index() const
	{
		return material_index_;
	}

	inline void pmx_material_morph_offset_impl::set_material_index(int32_t index)
	{
		material_index_ = index;
	}

	inline pmx_material_morph_offset::blend_mode pmx_material_morph_offset_impl::get_blend_mode() const
	{
		return blend_mode_;
	}

	inline void pmx_material_morph_offset_impl::set_blend_mode(const blend_mode& mode)
	{
		blend_mode_ = mode;
	}

	inline const std::array<float, 4>& pmx_material_morph_offset_impl::get_diffuse() const
	{
		return diffuse_;
	}

	inline void pmx_material_morph_offset_impl::set_diffuse(const std::array<float, 4>& diffuse)
	{
		diffuse_ = diffuse;
	}

	inline const std::array<float, 3>& pmx_material_morph_offset_impl::get_specular() const
	{
		return specular_;
	}

	inline void pmx_material_morph_offset_impl::set_specular(const std::array<float, 3>& specular)
	{
		specular_ = specular;
	}

	inline float pmx_material_morph_offset_impl::get_specular_intensity() const
	{
		return specular_intensity_;
	}

	inline void pmx_material_morph_offset_impl::set_specular_intensity(float specular_intensity)
	{
		specular_intensity_ = specular_intensity;
	}

	inline const std::array<float, 3>& pmx_material_morph_offset_impl::get_ambient() const
	{
		return ambient_;
	}

	inline void pmx_material_morph_offset_impl::set_ambient(const std::array<float, 3>& ambient)
	{
		ambient_ = ambient;
	}

	inline const std::array<float, 4>& pmx_material_morph_offset_impl::get_edge_color() const
	{
		return edge_color_;
	}

	inline void pmx_material_morph_offset_impl::set_edge_color(const std::array<float, 4>& edge_color)
	{
		edge_color_ = edge_color;
	}

	inline float pmx_material_morph_offset_impl::get_edge_size() const
	{
		return edge_size_;
	}

	inline void pmx_material_morph_offset_impl::set_edge_size(float edge_size)
	{
		edge_size_ = edge_size;
	}

	inline const std::array<float, 4>& pmx_material_morph_offset_impl::get_texture_tint() const
	{
		return texture_tint_;
	}

	inline void pmx_material_morph_offset_impl::set_texture_tint(const std::array<float, 4>& texture_tint)
	{
		texture_tint_ = texture_tint;
	}

	inline const std::array<float, 4>& pmx_material_morph_offset_impl::get_environment_tint() const
	{
		return environment_tint_;
	}

	inline void pmx_material_morph_offset_impl::set_environment_tint(const std::array<float, 4>& environment_tint)
	{
		environment_tint_ = environment_tint;
	}

	inline const std::array<float, 4>& pmx_material_morph_offset_impl::get_toon_tint() const
	{
		return toon_tint_;
	}

	inline void pmx_material_morph_offset_impl::set_toon_tint(const std::array<float, 4>& toon_tint)
	{
		toon_tint_ = toon_tint;
	}

	bool pmx_material_morph_offset_impl::read_from_file(const file& file)
	{
		if (!model_description_->read_material_index(file, material_index_))
			return false;
		if (!file.read_elements(blend_mode_))
			return false;
		if (!file.read_elements(diffuse_))
			return false;
		if (!file.read_elements(specular_))
			return false;
		if (!file.read_elements(specular_intensity_))
			return false;
		if (!file.read_elements(ambient_))
			return false;
		if (!file.read_elements(edge_color_))
			return false;
		if (!file.read_elements(edge_size_))
			return false;
		if (!file.read_elements(texture_tint_))
			return false;
		if (!file.read_elements(environment_tint_))
			return false;
		if (!file.read_elements(toon_tint_))
			return false;
		return true;
	}

	inline bool pmx_material_morph_offset_impl::write_to_file(const file& file) const
	{
		// TODO: write to file
		return false;
	}

	inline int32_t pmx_flip_morph_offset_impl::get_morph_index() const
	{
		return morph_index_;
	}

	inline void pmx_flip_morph_offset_impl::set_morph_index(int32_t morph_index)
	{
		morph_index_ = morph_index;
	}

	inline float pmx_flip_morph_offset_impl::get_morph_weight() const
	{
		return morph_weight_;
	}

	inline void pmx_flip_morph_offset_impl::set_morph_weight(float morph_weight)
	{
		morph_weight_ = morph_weight;
	}

	bool pmx_flip_morph_offset_impl::read_from_file(const file& file)
	{
		if (!model_description_->read_morph_index(file, morph_index_))
			return false;
		if (!file.read_elements(morph_weight_))
			return false;
		return true;
	}

	bool pmx_flip_morph_offset_impl::write_to_file(const file& file) const
	{
		// TODO: write to file
		return false;
	}

	inline int32_t pmx_impulse_morph_offset_impl::get_rigid_body_index() const
	{
		return rigid_body_index_;
	}

	inline void pmx_impulse_morph_offset_impl::set_rigid_body_index(int32_t rigid_body_index)
	{
		rigid_body_index_ = rigid_body_index;
	}

	inline bool pmx_impulse_morph_offset_impl::is_local() const
	{
		return is_local_;
	}

	inline void pmx_impulse_morph_offset_impl::set_local(bool value)
	{
		is_local_ = value;
	}

	inline const std::array<float, 3>& pmx_impulse_morph_offset_impl::get_movement_speed() const
	{
		return movement_speed_;
	}

	inline void pmx_impulse_morph_offset_impl::set_movement_speed(const std::array<float, 3>& speed)
	{
		movement_speed_ = speed;
	}

	inline const std::array<float, 3>& pmx_impulse_morph_offset_impl::get_rotation_torque() const
	{
		return rotation_torque_;
	}

	inline void pmx_impulse_morph_offset_impl::set_rotation_torque(const std::array<float, 3>& torque)
	{
		rotation_torque_ = torque;
	}

	bool pmx_impulse_morph_offset_impl::read_from_file(const file& file)
	{
		if (!model_description_->read_rigid_body_index(file, rigid_body_index_))
			return false;
		if (!file.read_elements(is_local_))
			return false;
		if (!file.read_elements(movement_speed_))
			return false;
		if (!file.read_elements(rotation_torque_))
			return false;
		return true;
	}

	bool pmx_impulse_morph_offset_impl::write_to_file(const file& file) const
	{
		// TODO: write to file
		return false;
	}

	constexpr size_t mapping_morph_type(pmx_morph::morph_type morph_type)
	{
		const auto result = static_cast<size_t>(morph_type);
		if (result > 3 && result < 8)
		{
			return 3;
		}
		if (result >= 8)
		{
			return result - 4;
		}
		return result;
	}

	inline std::string pmx_morph_impl::get_morph_name_local() const
	{
		return std::string{morph_name_local_.begin(), morph_name_local_.end()};
	}

	inline void pmx_morph_impl::set_morph_name_local(const std::string& name)
	{
		morph_name_local_ = std::u8string{ name.begin(),name.end() };
	}

	inline std::string pmx_morph_impl::get_morph_name_universal() const
	{
		return std::string{morph_name_universal_.begin(), morph_name_universal_.end()};
	}

	inline void pmx_morph_impl::set_morph_name_universal(const std::string& name)
	{
		morph_name_universal_ = std::u8string{ name.begin(),name.end() };
	}

	inline pmx_morph::display_panel_type pmx_morph_impl::get_display_panel_type() const
	{
		return display_panel_type_;
	}

	inline void pmx_morph_impl::set_display_panel_type(display_panel_type type)
	{
		display_panel_type_ = type;
	}

	inline pmx_morph::morph_type pmx_morph_impl::get_morph_offset_type() const
	{
		return morph_type_;
	}

	void pmx_morph_impl::set_morph_offset_type(const morph_type type)
	{
		if(morph_type_ == type)
			return;

		morph_type_ = type;
		switch (morph_type_)
		{
		case morph_type::GROUP:
		{
			if(offset_data_.index() != mapping_morph_type(morph_type::GROUP))
				offset_data_.emplace<pmx_group_morph_array_impl>(model_description_);
			break;
		}
		case morph_type::VERTEX:
		{
			if (offset_data_.index() != mapping_morph_type(morph_type::VERTEX))
				offset_data_.emplace<pmx_vertex_morph_array_impl>(model_description_);
			break;
		}
		case morph_type::BONE:
		{
			if (offset_data_.index() != mapping_morph_type(morph_type::BONE))
				offset_data_.emplace<pmx_bone_morph_array_impl>(model_description_);
			break;
		}
		case morph_type::UV0:
		case morph_type::UV1:
		case morph_type::UV2:
		case morph_type::UV3:
		case morph_type::UV4:
		{
			if (offset_data_.index() != mapping_morph_type(morph_type::UV0))
				offset_data_.emplace<pmx_uv_morph_array_impl>(model_description_);
			break;
		}
		case morph_type::MATERIAL:
		{
			if (offset_data_.index() != mapping_morph_type(morph_type::MATERIAL))
				offset_data_.emplace<pmx_material_morph_array_impl>(model_description_);
			break;
		}
		case morph_type::FLIP:
		{
			if (offset_data_.index() != mapping_morph_type(morph_type::FLIP))
				offset_data_.emplace<pmx_flip_morph_array_impl>(model_description_);
			break;
		}
		case morph_type::IMPULSE:
		{
			if (offset_data_.index() != mapping_morph_type(morph_type::IMPULSE))
				offset_data_.emplace<pmx_impulse_morph_array_impl>(model_description_);
			break;
		}
		}
	}

	const pmx_morph::pmx_morph_offset_array& pmx_morph_impl::get_morph_offset_array() const
	{
		if(!std::holds_alternative<std::monostate>(offset_data_))
		switch (morph_type_)
		{
		case morph_type::GROUP:
		{
			if (constexpr auto morph_type_id = mapping_morph_type(morph_type::GROUP); offset_data_.index() == morph_type_id)
			{
				return std::get<morph_type_id>(offset_data_);
			}
		}
		case morph_type::VERTEX:
		{
			if (constexpr auto morph_type_id = mapping_morph_type(morph_type::VERTEX); offset_data_.index() == morph_type_id)
			{
				return std::get<morph_type_id>(offset_data_);
			}
		}
		case morph_type::BONE:
		{
			if (constexpr auto morph_type_id = mapping_morph_type(morph_type::BONE); offset_data_.index() == morph_type_id)
			{
				return std::get<morph_type_id>(offset_data_);
			}
		}
		case morph_type::UV0:
		case morph_type::UV1:
		case morph_type::UV2:
		case morph_type::UV3:
		case morph_type::UV4:
		{
			if (constexpr auto morph_type_id = mapping_morph_type(morph_type::UV0); offset_data_.index() == morph_type_id)
			{
				return std::get<morph_type_id>(offset_data_);
			}
		}
		case morph_type::MATERIAL:
		{
			if (constexpr auto morph_type_id = mapping_morph_type(morph_type::MATERIAL); offset_data_.index() == morph_type_id)
			{
				return std::get<morph_type_id>(offset_data_);
			}
		}
		case morph_type::FLIP:
		{
			if (constexpr auto morph_type_id = mapping_morph_type(morph_type::FLIP); offset_data_.index() == morph_type_id)
			{
				return std::get<morph_type_id>(offset_data_);
			}
		}
		case morph_type::IMPULSE:
		{
			if (constexpr auto morph_type_id = mapping_morph_type(morph_type::IMPULSE); offset_data_.index() == 0)
			{
				return std::get<morph_type_id>(offset_data_);
			}
		}
		}
		// It shouldn't run here.
		ELOGE << "pmx_morph_impl::get_morph_offset_array: Can't cast offset_data by type!";
		return std::get<1>(offset_data_);
	}

	pmx_morph::pmx_morph_offset_array& pmx_morph_impl::mutable_morph_offset_array()
	{
		if (!std::holds_alternative<std::monostate>(offset_data_))
		switch (morph_type_)
		{
		case morph_type::GROUP:
		{
			if (constexpr auto morph_type_id = mapping_morph_type(morph_type::GROUP); offset_data_.index() == morph_type_id)
			{
				return std::get<morph_type_id>(offset_data_);
			}
		}
		case morph_type::VERTEX:
		{
			if (constexpr auto morph_type_id = mapping_morph_type(morph_type::VERTEX); offset_data_.index() == morph_type_id)
			{
				return std::get<morph_type_id>(offset_data_);
			}
		}
		case morph_type::BONE:
		{
			if (constexpr auto morph_type_id = mapping_morph_type(morph_type::BONE); offset_data_.index() == morph_type_id)
			{
				return std::get<morph_type_id>(offset_data_);
			}
		}
		case morph_type::UV0:
		case morph_type::UV1:
		case morph_type::UV2:
		case morph_type::UV3:
		case morph_type::UV4:
		{
			if (constexpr auto morph_type_id = mapping_morph_type(morph_type::UV0); offset_data_.index() == morph_type_id)
			{
				return std::get<morph_type_id>(offset_data_);
			}
		}
		case morph_type::MATERIAL:
		{
			if (constexpr auto morph_type_id = mapping_morph_type(morph_type::MATERIAL); offset_data_.index() == morph_type_id)
			{
				return std::get<morph_type_id>(offset_data_);
			}
		}
		case morph_type::FLIP:
		{
			if (constexpr auto morph_type_id = mapping_morph_type(morph_type::FLIP); offset_data_.index() == morph_type_id)
			{
				return std::get<morph_type_id>(offset_data_);
			}
		}
		case morph_type::IMPULSE:
		{
			if (constexpr auto morph_type_id = mapping_morph_type(morph_type::IMPULSE); offset_data_.index() == 0)
			{
				return std::get<morph_type_id>(offset_data_);
			}
		}
		}
		// It shouldn't run here.
		ELOGE << "pmx_morph_impl::mutable_morph_offset_array: Can't cast offset_data by type!";
		return std::get<1>(offset_data_);
	}

	bool pmx_morph_impl::read_from_file(const file& file)
	{
		if (!model_description_->read_text(file, morph_name_local_))
			return false;
		if (!model_description_->read_text(file, morph_name_universal_))
			return false;
		if (!file.read_elements(display_panel_type_))
			return false;
		if (!file.read_elements(morph_type_))
			return false;
		set_morph_offset_type(morph_type_);

		auto offset_count = Int32();
		if (!file.read_elements(offset_count))
			return false;
		if(offset_count == 0)
			return true;

		switch (morph_type_)
		{
		case morph_type::GROUP:
		{
			constexpr auto morph_type_id = mapping_morph_type(morph_type::GROUP);
			auto& offset_data = std::get<morph_type_id>(offset_data_);
			offset_data.clear();
			for (Int32 offset_index = 0; offset_index < offset_count; offset_index++)
			{
				if (auto& offset = offset_data.add_impl(); !offset.read_from_file(file))
					return false;
			}
			break;
		}
		case morph_type::VERTEX:
		{
			constexpr auto morph_type_id = mapping_morph_type(morph_type::VERTEX);
			auto& offset_data = std::get<morph_type_id>(offset_data_);
			offset_data.clear();
			for (Int32 offset_index = 0; offset_index < offset_count; offset_index++)
			{
				if (auto& offset = offset_data.add_impl(); !offset.read_from_file(file))
					return false;
			}
			break;
		}
		case morph_type::BONE:
		{
			constexpr auto morph_type_id = mapping_morph_type(morph_type::BONE);
			auto& offset_data = std::get<morph_type_id>(offset_data_);
			offset_data.clear();
			for (Int32 offset_index = 0; offset_index < offset_count; offset_index++)
			{
				if (auto& offset = offset_data.add_impl(); !offset.read_from_file(file))
					return false;
			}
			break;
		}
		case morph_type::UV0:
		case morph_type::UV1:
		case morph_type::UV2:
		case morph_type::UV3:
		case morph_type::UV4:
		{
			constexpr auto morph_type_id = mapping_morph_type(morph_type::UV0);
			auto& offset_data = std::get<morph_type_id>(offset_data_);
			offset_data.clear();
			for (Int32 offset_index = 0; offset_index < offset_count; offset_index++)
			{
				if (auto& offset = offset_data.add_impl(); !offset.read_from_file(file))
					return false;
			}
			break;
		}
		case morph_type::MATERIAL:
		{
			constexpr auto morph_type_id = mapping_morph_type(morph_type::MATERIAL);
			auto& offset_data = std::get<morph_type_id>(offset_data_);
			offset_data.clear();
			for (Int32 offset_index = 0; offset_index < offset_count; offset_index++)
			{
				if (auto& offset = offset_data.add_impl(); !offset.read_from_file(file))
					return false;
			}
			break;
		}
		case morph_type::FLIP:
		{
			constexpr auto morph_type_id = mapping_morph_type(morph_type::FLIP);
			auto& offset_data = std::get<morph_type_id>(offset_data_);
			offset_data.clear();
			for (Int32 offset_index = 0; offset_index < offset_count; offset_index++)
			{
				if (auto& offset = offset_data.add_impl(); !offset.read_from_file(file))
					return false;
			}
			break;
		}
		case morph_type::IMPULSE:
		{
			constexpr auto morph_type_id = mapping_morph_type(morph_type::IMPULSE);
			auto& offset_data = std::get<morph_type_id>(offset_data_);
			offset_data.clear();
			for (Int32 offset_index = 0; offset_index < offset_count; offset_index++)
			{
				if (auto& offset = offset_data.add_impl(); !offset.read_from_file(file))
					return false;
			}
			break;
		}
		}
		return true;
	}

	bool pmx_morph_impl::write_to_file(const file& file) const
	{
		// TODO: write to file
		return false;
	}
}
