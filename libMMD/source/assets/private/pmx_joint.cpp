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
	inline std::string pmx_joint_impl::get_joint_name_local() const
	{
		return std::string{joint_name_local_.begin(), joint_name_local_.end()};
	}

	inline void pmx_joint_impl::set_joint_name_local(const std::string& name)
	{
		joint_name_local_ = std::u8string{ name.begin(),name.end() };
	}

	inline std::string pmx_joint_impl::get_joint_name_universal() const
	{
		return std::string{joint_name_universal_.begin(), joint_name_universal_.end()};
	}

	inline void pmx_joint_impl::set_joint_name_universal(const std::string& name)
	{
		joint_name_universal_ = std::u8string{ name.begin(),name.end() };
	}

	inline pmx_joint::joint_type pmx_joint_impl::get_joint_type() const
	{
		return joint_type_;
	}

	inline void pmx_joint_impl::set_joint_type(joint_type type)
	{
		joint_type_ = type;
	}

	inline int32_t pmx_joint_impl::get_rigid_body_index_a() const
	{
		return rigid_body_index_a_;
	}

	inline void pmx_joint_impl::set_rigid_body_index_a(int32_t index)
	{
		rigid_body_index_a_ = index;
	}

	inline int32_t pmx_joint_impl::get_rigid_body_index_b() const
	{
		return rigid_body_index_b_;
	}

	inline void pmx_joint_impl::set_rigid_body_index_b(int32_t index)
	{
		rigid_body_index_b_ = index;
	}

	inline const std::array<float, 3>& pmx_joint_impl::get_position() const
	{
		return position_;
	}

	inline void pmx_joint_impl::set_position(const std::array<float, 3>& position)
	{
		position_ = position;
	}

	inline const std::array<float, 3>& pmx_joint_impl::get_rotation() const
	{
		return rotation_;
	}

	inline void pmx_joint_impl::set_rotation(const std::array<float, 3>& rotation)
	{
		rotation_ = rotation;
	}

	inline const std::array<float, 3>& pmx_joint_impl::get_position_minimum() const
	{
		return position_minimum_;
	}

	inline void pmx_joint_impl::set_position_minimum(const std::array<float, 3>& minimum)
	{
		position_minimum_ = minimum;
	}

	inline const std::array<float, 3>& pmx_joint_impl::get_position_maximum() const
	{
		return position_maximum_;
	}

	inline void pmx_joint_impl::set_position_maximum(const std::array<float, 3>& maximum)
	{
		position_maximum_ = maximum;
	}

	inline const std::array<float, 3>& pmx_joint_impl::get_rotation_minimum() const
	{
		return rotation_minimum_;
	}

	inline void pmx_joint_impl::set_rotation_minimum(const std::array<float, 3>& minimum)
	{
		rotation_minimum_ = minimum;
	}

	inline const std::array<float, 3>& pmx_joint_impl::get_rotation_maximum() const
	{
		return rotation_maximum_;
	}

	inline void pmx_joint_impl::set_rotation_maximum(const std::array<float, 3>& maximum)
	{
		rotation_maximum_ = maximum;
	}

	inline const std::array<float, 3>& pmx_joint_impl::get_position_spring() const
	{
		return position_spring_;
	}

	inline void pmx_joint_impl::set_position_spring(const std::array<float, 3>& spring)
	{
		position_spring_ = spring;
	}

	inline const std::array<float, 3>& pmx_joint_impl::get_rotation_spring() const
	{
		return rotation_spring_;
	}

	inline void pmx_joint_impl::set_rotation_spring(const std::array<float, 3>& spring)
	{
		rotation_spring_ = spring;
	}

	bool pmx_joint_impl::read_from_file(const file& file)
	{
		if (!model_description_->read_text(file, joint_name_local_))
			return false;
		if (!model_description_->read_text(file, joint_name_universal_))
			return false;
		if (!file.read_elements(joint_type_))
			return false;
		if (!model_description_->read_rigid_body_index(file, rigid_body_index_a_))
			return false;
		if (!model_description_->read_rigid_body_index(file, rigid_body_index_b_))
			return false;
		if (!file.read_elements(position_))
			return false;
		if (!file.read_elements(rotation_))
			return false;
		if (!file.read_elements(position_minimum_))
			return false;
		if (!file.read_elements(position_maximum_))
			return false;
		if (!file.read_elements(rotation_minimum_))
			return false;
		if (!file.read_elements(rotation_maximum_))
			return false;
		if (!file.read_elements(position_spring_))
			return false;
		if (!file.read_elements(rotation_spring_))
			return false;
		return true;
	}

	bool pmx_joint_impl::write_to_file(const file& file) const
	{
		// TODO: write to file
		return false;
	}
}
