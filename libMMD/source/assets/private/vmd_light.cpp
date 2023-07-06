/**************************************************************************

Copyright:Copyright(c) 2022-present, Aimidi & Walter White & CMT contributors.
Author:			Aimidi
Date:			2022/7/15
File:			vmd_light_animation.cpp
Description:	MMD style lighting animation

**************************************************************************/

#include "vmd_light.h"

namespace libmmd
{
	inline const std::array<float, 3>& vmd_light_key_frame_impl::get_color() const
	{
		return color_;
	}

	inline void vmd_light_key_frame_impl::set_color(const std::array<float, 3>& color)
	{
		color_ = color;
	}

	inline const std::array<float, 3>& vmd_light_key_frame_impl::get_position() const
	{
		return position_;
	}

	inline void vmd_light_key_frame_impl::set_position(const std::array<float, 3>& position)
	{
		position_ = position;
	}

	bool vmd_light_key_frame_impl::read_from_file(const file& file)
	{
		if (!file.read_elements(frame_at_))
			return false;
		if (!file.read_elements(color_))
			return false;
		if (!file.read_elements(position_))
			return false;
		return true;
	}

	bool vmd_light_key_frame_impl::write_to_file(const file& file) const
	{
		if (!file.write_elements(frame_at_))
			return false;
		if (!file.write_elements(color_))
			return false;
		if (!file.write_elements(position_))
			return false;
		return true;
	}
}
