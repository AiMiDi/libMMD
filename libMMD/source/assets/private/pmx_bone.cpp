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
	inline int32_t pmx_IK_link_impl::get_bone_index() const
	{
		return bone_index_;
	}

	inline void pmx_IK_link_impl::set_bone_index(const int32_t index)
	{
		bone_index_ = index;
	}

	inline bool pmx_IK_link_impl::have_limits() const
	{
		return have_limits_;
	}

	inline void pmx_IK_link_impl::set_have_limits(const bool value)
	{
		have_limits_ = value;
	}

	inline const std::array<float, 3>& pmx_IK_link_impl::get_limit_min() const
	{
		return limit_min_;
	}

	inline void pmx_IK_link_impl::set_limit_min(const std::array<float, 3>& min)
	{
		limit_min_ = min;
	}

	inline const std::array<float, 3>& pmx_IK_link_impl::get_limit_max() const
	{
		return limit_max_;
	}

	inline void pmx_IK_link_impl::set_limit_max(const std::array<float, 3>& max)
	{
		limit_max_ = max;
	}

	bool pmx_IK_link_impl::read_from_file(const file& file)
	{
		if (!model_description_->read_bone_index(file, bone_index_))
			return false;
		if (!file.read_elements(have_limits_))
			return false;
		if (have_limits_)
		{
			if (!file.read_elements(limit_min_))
				return false;
			if (!file.read_elements(limit_max_))
				return false;
		}
		return true;
	}

	bool pmx_IK_link_impl::write_to_file(const file& file) const
	{
		// TODO: write to file
		return false;
	}

	inline std::string pmx_bone_impl::get_bone_name_local() const
	{
		return std::string{bone_name_local_.begin(), bone_name_local_.end()};
	}

	inline void pmx_bone_impl::set_bone_name_local(const std::string& name)
	{
		bone_name_local_ = std::u8string(name.begin(), name.end());
	}

	inline std::string pmx_bone_impl::get_bone_name_universal() const
	{
		return std::string{bone_name_universal_.begin(), bone_name_universal_.end()};
	}

	inline void pmx_bone_impl::set_bone_name_universal(const std::string& name)
	{
		bone_name_universal_ = std::u8string(name.begin(), name.end());
	}

	inline const std::array<float, 3>& pmx_bone_impl::get_position() const
	{
		return position_;
	}

	inline void pmx_bone_impl::set_position(const std::array<float, 3>& position)
	{
		position_ = position;
	}

	inline int32_t pmx_bone_impl::get_parent_bone_index() const
	{
		return parent_bone_index_;
	}

	inline void pmx_bone_impl::set_parent_bone_index(const int32_t index)
	{
		parent_bone_index_ = index;
	}

	inline int32_t pmx_bone_impl::get_layer() const
	{
		return layer_;
	}

	inline void pmx_bone_impl::set_layer(const int32_t layer)
	{
		layer_ = layer;
	}

	inline bool pmx_bone_impl::is_rotatable() const
	{
		return bone_flags_.rotatable;
	}

	inline void pmx_bone_impl::set_is_rotatable(const bool value)
	{
		bone_flags_.rotatable = value;
	}

	inline bool pmx_bone_impl::is_translatable() const
	{
		return bone_flags_.translatable;
	}

	inline void pmx_bone_impl::set_is_translatable(const bool value)
	{
		bone_flags_.translatable = value;
	}

	inline bool pmx_bone_impl::is_visible() const
	{
		return bone_flags_.is_visible;
	}

	inline void pmx_bone_impl::set_is_visible(const bool value)
	{
		bone_flags_.is_visible = value;
	}

	inline bool pmx_bone_impl::is_enabled() const
	{
		return bone_flags_.enabled;
	}

	inline void pmx_bone_impl::set_is_enabled(const bool value)
	{
		bone_flags_.enabled = value;
	}

	inline bool pmx_bone_impl::is_physics_after_deform() const
	{
		return bone_flags_.physics_after_deform;
	}

	inline void pmx_bone_impl::set_is_physics_after_deform(const bool value)
	{
		bone_flags_.physics_after_deform = value;
	}

	inline bool pmx_bone_impl::have_external_parent_deform() const
	{
		return bone_flags_.external_parent_deform;
	}

	inline void pmx_bone_impl::set_have_external_parent_deform(const bool value)
	{
		bone_flags_.external_parent_deform = value;
	}

	inline bool pmx_bone_impl::is_tail_use_index() const
	{
		return bone_flags_.indexed_tail_position;
	}

	inline void pmx_bone_impl::set_is_tail_use_index(const bool value)
	{
		bone_flags_.indexed_tail_position = value;
	}

	inline const std::array<float, 3>& pmx_bone_impl::get_tail_position() const
	{
		return tail_position_;
	}

	inline void pmx_bone_impl::set_tail_position(const std::array<float, 3>& position)
	{
		tail_position_ = position;
	}

	inline int32_t pmx_bone_impl::get_tail_index() const
	{
		return tail_index_;
	}

	inline void pmx_bone_impl::set_tail_index(const int32_t index)
	{
		tail_index_ = index;
	}

	inline bool pmx_bone_impl::have_inherit_rotation() const
	{
		return bone_flags_.inherit_rotation;
	}

	inline void pmx_bone_impl::set_have_inherit_rotation(const bool value)
	{
		bone_flags_.inherit_rotation = value;
	}

	inline bool pmx_bone_impl::have_inherit_translation() const
	{
		return bone_flags_.inherit_translation;
	}

	inline void pmx_bone_impl::set_have_inherit_translation(const bool value)
	{
		bone_flags_.inherit_translation = value;
	}

	inline int32_t pmx_bone_impl::get_inherit_bone_parent_index() const
	{
		return inherit_bone_parent_index_;
	}

	inline void pmx_bone_impl::set_inherit_bone_parent_index(const int32_t index)
	{
		inherit_bone_parent_index_ = index;
	}

	inline float pmx_bone_impl::get_inherit_bone_parent_influence() const
	{
		return inherit_bone_parent_influence_;
	}

	inline void pmx_bone_impl::set_inherit_bone_parent_influence(const float influence)
	{
		inherit_bone_parent_influence_ = influence;
	}

	inline bool pmx_bone_impl::have_fixed_axis() const
	{
		return bone_flags_.fixed_axis;
	}

	inline void pmx_bone_impl::set_have_fixed_axis(const bool value)
	{
		bone_flags_.fixed_axis = value;
	}

	inline const std::array<float, 3>& pmx_bone_impl::get_bone_fixed_axis() const
	{
		return bone_fixed_axis_;
	}

	inline void pmx_bone_impl::set_bone_fixed_axis(const std::array<float, 3>& axis)
	{
		bone_fixed_axis_ = axis;
	}

	inline bool pmx_bone_impl::have_local_coordinate() const
	{
		return bone_flags_.local_coordinate;
	}

	inline void pmx_bone_impl::set_have_local_coordinate(const bool value)
	{
		bone_flags_.local_coordinate = value;
	}

	inline const std::array<float, 3>& pmx_bone_impl::get_bone_local_coordinate_x() const
	{
		return bone_local_x_;
	}

	inline void pmx_bone_impl::set_bone_local_coordinate_x(const std::array<float, 3>& x)
	{
		bone_local_x_ = x;
	}

	inline const std::array<float, 3>& pmx_bone_impl::get_bone_local_coordinate_z() const
	{
		return bone_local_z_;
	}

	inline void pmx_bone_impl::set_bone_local_coordinate_z(const std::array<float, 3>& z)
	{
		bone_local_z_ = z;
	}

	inline bool pmx_bone_impl::is_IK() const
	{
		return bone_flags_.is_IK;
	}

	inline void pmx_bone_impl::set_is_IK(const bool value)
	{
		bone_flags_.is_IK = value;
	}

	inline int32_t pmx_bone_impl::get_IK_target_index() const
	{
		return IK_target_index_;
	}

	inline void pmx_bone_impl::set_IK_target_index(const int32_t index)
	{
		IK_target_index_ = index;
	}

	inline int32_t pmx_bone_impl::get_IK_loop_count() const
	{
		return IK_loop_count_;
	}

	inline void pmx_bone_impl::set_IK_loop_count(int32_t count)
	{
		IK_loop_count_ = count;
	}

	inline float pmx_bone_impl::get_IK_limit_radian() const
	{
		return IK_limit_radian_;
	}

	inline void pmx_bone_impl::set_IK_limit_radian(float radian)
	{
		IK_limit_radian_ = radian;
	}

	inline const pmx_bone::pmx_IK_link_array& pmx_bone_impl::get_IK_link_array() const
	{
		return IK_links_;
	}

	inline pmx_bone::pmx_IK_link_array& pmx_bone_impl::mutable_IK_link_array()
	{
		return IK_links_;
	}

	bool pmx_bone_impl::read_from_file(const file& file)
	{
		if (!model_description_->read_text(file, bone_name_local_))
			return false;
		if (!model_description_->read_text(file, bone_name_universal_))
			return false;
		if (!file.read_elements(position_))
			return false;
		if (!model_description_->read_bone_index(file, parent_bone_index_))
			return false;
		if (!file.read_elements(layer_))
			return false;
		if (!file.read_elements(bone_flags_))
			return false;
		if (bone_flags_.indexed_tail_position == 0)
		{
			if (!file.read_elements(tail_position_))
				return false;
		}
		else if (bone_flags_.indexed_tail_position == 1)
		{
			if (!model_description_->read_bone_index(file, tail_index_))
				return false;
		}
		if (bone_flags_.inherit_rotation || bone_flags_.inherit_translation)
		{
			if (!model_description_->read_bone_index(file, inherit_bone_parent_index_))
				return false;
			if (!file.read_elements(inherit_bone_parent_influence_))
				return false;
		}
		if (bone_flags_.fixed_axis)
		{
			if (!file.read_elements(bone_fixed_axis_))
				return false;
		}
		if (bone_flags_.local_coordinate)
		{
			if (!file.read_elements(bone_local_x_))
				return false;
			if (!file.read_elements(bone_local_z_))
				return false;
		}
		if (bone_flags_.external_parent_deform)
		{
			if (!file.seek(4))
				return false;
		}
		if (bone_flags_.is_IK)
		{
			if (!model_description_->read_bone_index(file, IK_target_index_))
				return false;
			if (!file.read_elements(IK_loop_count_))
				return false;
			if (!file.read_elements(IK_limit_radian_))
				return false;

			auto IK_link_count = int32_t{};
			if (!file.read_elements(IK_link_count))
				return false;

			for (auto IK_link_index = decltype(IK_link_count){}; IK_link_index < IK_link_count; IK_link_index++)
			{
				if (auto& IK_link = IK_links_.add_impl(); !IK_link.read_from_file(file))
					return false;
			}
		}
		return true;
	}

	bool pmx_bone_impl::write_to_file(const file& file) const
	{
		// TODO: write to file
		return false;
	}
}
