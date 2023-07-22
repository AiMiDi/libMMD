/**************************************************************************

Copyright:Copyright(c) 2022-present, Aimidi & Walter White & CMT contributors.
Author:			Aimidi
Date:			2022/7/15
File:			vmd_shadow_animation.cpp
Description:	MMD style shadow animation

**************************************************************************/

#include "vmd_shadow.h"

namespace libmmd
{
	bool vmd_shadow_key_frame_impl::read_from_file(const file& file)
	{
		if (!file.read_elements(frame_at_))
			return false;
		if (!file.read_elements(shadow_type_))
			return false;
		if (!file.read_elements(distance_))
			return false;
		return true;
	}

	bool vmd_shadow_key_frame_impl::write_to_file(const file& file) const
	{
		if (!file.write_elements(frame_at_))
			return false;
		if (!file.write_elements(shadow_type_))
			return false;
		if (!file.write_elements(distance_))
			return false;
		return true;
	}

	vmd_shadow_key_frame::shadow_type vmd_shadow_key_frame_impl::get_shadow_type() const
	{
		return shadow_type_;
	}

	void vmd_shadow_key_frame_impl::set_shadow_type(const shadow_type type)
	{
		shadow_type_ = type;
	}

	float vmd_shadow_key_frame_impl::get_distance() const
	{
		return distance_;
	}

	void vmd_shadow_key_frame_impl::set_distance(float distance)
	{
		distance_ = distance;
	}
}
