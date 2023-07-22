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
	inline std::string vmd_morph_key_frame_impl::get_morph_name() const
	{
		return std::string{morph_name_.begin(), morph_name_.end()};
	}

	inline void vmd_morph_key_frame_impl::set_morph_name(const std::string& name)
	{
		morph_name_ = std::u8string{ name.begin(), name.end() };
	}

	inline float vmd_morph_key_frame_impl::get_weight() const
	{
		return weight_;
	}

	inline void vmd_morph_key_frame_impl::set_weight(float weight)
	{
		weight_ = weight;
	}

	bool vmd_morph_key_frame_impl::read_from_file(const file& file)
	{
		Char morph_name[15] = { '\0' };
		if (!file.read_elements(morph_name, 15LLU))
			return false;
		if (!file.read_elements(frame_at_))
			return false;
		if (!file.read_elements(weight_))
			return false;
		morph_name_ = code_converter::shift_jis_to_utf8(morph_name);
		return true;
	}

	bool vmd_morph_key_frame_impl::write_to_file(const file& file) const
	{
		std::string morph_name = code_converter::utf8_to_shift_jis(morph_name_);
		morph_name.resize(15, '\0');
		if (!file.write_elements(morph_name.c_str(), 15LLU))
			return false;
		if (!file.write_elements(frame_at_))
			return false;
		if (!file.write_elements(weight_))
			return false;
		return true;
	}
}