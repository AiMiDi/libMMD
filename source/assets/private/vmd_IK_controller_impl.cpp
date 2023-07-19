/**************************************************************************

Copyright:Copyright(c) 2022-present, Aimidi & Walter White & CMT contributors.
Author:			Aimidi
Date:			2022/7/14
File:			vmd_Ik_controller_animation.cpp
Description:	MMD style IK enable information animation

**************************************************************************/

#include "vmd_IK_controller_impl.h"

namespace libmmd
{
	std::string vmd_IK_controller_impl::get_bone_name() const
	{
		return std::string{IK_bone_name_.begin(), IK_bone_name_.end()};
	}

	void vmd_IK_controller_impl::set_bone_name(const std::string& name)
	{
		IK_bone_name_ = std::u8string{ name.begin(), name.end() };
	}

	bool vmd_IK_controller_impl::is_IK_enable() const
	{
		return IK_enable_;
	}

	void vmd_IK_controller_impl::set_IK_enable(bool value)
	{
		IK_enable_ = value;
	}

	bool vmd_IK_controller_impl::read_from_file(const file& file)
	{
		Char ik_name[20]{ '\0' };
		if (!file.read_elements(ik_name, 20LLU))
			return false;
		if (!file.read_elements(IK_enable_))
			return false;
		IK_bone_name_ = code_converter::shift_jis_to_utf8(ik_name);
		return true;
	}

	bool vmd_IK_controller_impl::write_to_file(const file& file) const
	{
		const std::string name = code_converter::utf8_to_shift_jis(IK_bone_name_);
		if (!file.write_elements(name.data(), 20LLU))
			return false;
		if (!file.write_elements(IK_enable_))
			return false;
		return true;
	}
}
