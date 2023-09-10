/**************************************************************************

Copyright:Copyright(c) 2022-present, Aimidi & Walter White & CMT contributors.
Author:			Aimidi
Date:			2022/7/16
File:			vmd_bone_animation.cpp
Description:	MMD style bone animation

**************************************************************************/

#include "vmd_bone.h"

namespace libmmd
{
	 std::string vmd_bone_key_frame_impl::get_bone_name() const
	{
		return std::string{bone_name_.begin(), bone_name_.end()};
	}

	 void vmd_bone_key_frame_impl::set_bone_name(const std::string& name)
	{
		bone_name_ = std::u8string{ name.begin(), name.end() };
	}

	 const std::array<float, 3>& vmd_bone_key_frame_impl::get_position() const
	{
		return position_;
	}

	 void vmd_bone_key_frame_impl::set_position(const std::array<float, 3>& position)
	{
		position_ = position;
	}

	 const std::array<float, 4>& vmd_bone_key_frame_impl::get_rotation() const
	{
		return rotation_;
	}

	 void vmd_bone_key_frame_impl::set_rotation(const std::array<float, 4>& rotation)
	{
		rotation_ = rotation;
	}

	 const vmd_interpolator& vmd_bone_key_frame_impl::get_position_x_interpolator() const
	{
		return interpolator_position_x_;
	}

	 vmd_interpolator& vmd_bone_key_frame_impl::mutable_position_x_interpolator()
	{
		return interpolator_position_x_;
	}

	 const vmd_interpolator& vmd_bone_key_frame_impl::get_position_y_interpolator() const
	{
		return interpolator_position_y_;
	}

	 vmd_interpolator& vmd_bone_key_frame_impl::mutable_position_y_interpolator()
	{
		return interpolator_position_y_;
	}

	 const vmd_interpolator& vmd_bone_key_frame_impl::get_position_z_interpolator() const
	{
		return interpolator_position_z_;
	}

	 vmd_interpolator& vmd_bone_key_frame_impl::mutable_position_z_interpolator()
	{
		return interpolator_position_z_;
	}

	 const vmd_interpolator& vmd_bone_key_frame_impl::get_rotation_interpolator() const
	{
		return interpolator_rotation_;
	}

	 vmd_interpolator& vmd_bone_key_frame_impl::mutable_rotation_interpolator()
	{
		return interpolator_rotation_;
	}

	bool vmd_bone_key_frame_impl::read_from_file(const file& file)
	{
		Char bone_name[15] {'\0'};
		if (!file.read_elements(std::span(bone_name), 15LLU))
			return false;
		bone_name_ = code_converter::shift_jis_to_utf8(bone_name);
		if (!file.read_element(frame_at_))
			return false;
		if (!file.read_element(position_))
			return false;
		if (!file.read_element(rotation_))
			return false;
		if (!interpolator_position_x_.read_from_file(file))
			return false;
		if (!interpolator_position_y_.read_from_file(file))
			return false;
		if (!interpolator_position_z_.read_from_file(file))
			return false;
		if (!interpolator_rotation_.read_from_file(file))
			return false;
		return true;
	}

	bool vmd_bone_key_frame_impl::write_to_file(const file& file) const
	{
		std::string bone_name = code_converter::utf8_to_shift_jis(bone_name_);
		bone_name.resize(15, '\0');
		if (!file.write_elements(std::span(bone_name.begin(), bone_name.end()), 15LLU))
			return false;
		if (!file.write_element(frame_at_))
			return false;
		if (!file.write_element(position_))
			return false;
		if (!file.write_element(rotation_))
			return false;
		if (!interpolator_position_x_.write_to_file(file))
			return false;
		if (!interpolator_position_y_.write_to_file(file))
			return false;
		if (!interpolator_position_z_.write_to_file(file))
			return false;
		if (!interpolator_rotation_.write_to_file(file))
			return false;
		return true;
	}
}
