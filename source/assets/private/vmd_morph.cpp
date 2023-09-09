/**************************************************************************

Copyright:Copyright(c) 2022-present, Aimidi & Walter White & CMT contributors.
Author:			Aimidi
Date:			2022/7/15
File:			vmd_morph_animation.cpp
Description:	MMD style expression animation

**************************************************************************/

#include "vmd_morph.h"

namespace libmmd
{
	 std::string vmd_morph_key_frame_impl::get_morph_name() const
	{
		return std::string{morph_name_.begin(), morph_name_.end()};
	}

	 void vmd_morph_key_frame_impl::set_morph_name(const std::string& name)
	{
		morph_name_ = std::u8string{ name.begin(), name.end() };
	}

	 float vmd_morph_key_frame_impl::get_weight() const
	{
		return weight_;
	}

	 void vmd_morph_key_frame_impl::set_weight(float weight)
	{
		weight_ = weight;
	}

	bool vmd_morph_key_frame_impl::read_from_file(const file& file)
	{
		Char morph_name[15] = { '\0' };
		if (!file.read_elements(std::span(morph_name), 15LLU))
			return false;
		if (!file.read_element(frame_at_))
			return false;
		if (!file.read_element(weight_))
			return false;
		morph_name_ = code_converter::shift_jis_to_utf8(morph_name);
		return true;
	}

	bool vmd_morph_key_frame_impl::write_to_file(const file& file) const
	{
		std::string morph_name = code_converter::utf8_to_shift_jis(morph_name_);
		morph_name.resize(15, '\0');
		if (!file.write_elements(std::span(morph_name.begin(), morph_name.end()), 15LLU))
			return false;
		if (!file.write_element(frame_at_))
			return false;
		if (!file.write_element(weight_))
			return false;
		return true;
	}
}
