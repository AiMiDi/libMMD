/**************************************************************************

Copyright:Copyright(c) 2022-present, Aimidi & Walter White & CMT contributors.
Author:			Aimidi
Date:			2022/7/14
File:			vmd_model_controller_animation.cpp
Description:	MMD style model information animation

**************************************************************************/

#include "vmd_model_controller.h"

namespace libmmd
{
	bool vmd_model_controller_key_frame_impl::is_mode_show() const
	{
		return model_show_;
	}

	void vmd_model_controller_key_frame_impl::set_mode_show(bool value)
	{
		model_show_ = value;
	}

	const vmd_model_controller_key_frame::vmd_IK_controller_array& vmd_model_controller_key_frame_impl::
	get_vmd_IK_controller_array() const
	{
		return IK_controller_array;
	}

	vmd_model_controller_key_frame::vmd_IK_controller_array& vmd_model_controller_key_frame_impl::
	mutable_vmd_IK_controller_array()
	{
		return IK_controller_array;
	}

	bool vmd_model_controller_key_frame_impl::read_from_file(const file& file)
	{
		if (!file.read_element(frame_at_))
			return false;
		if (!file.read_element(model_show_))
			return false;
		UInt32 IK_controller_number = 0;
		if (!file.read_element(IK_controller_number))
			return false;
		IK_controller_array.read_from_file(file);
		return true;
	}

	bool vmd_model_controller_key_frame_impl::write_to_file(const file& file) const
	{
		if (!file.write_element(frame_at_))
			return false;
		if (!file.write_element(model_show_))
			return false;
		if (!IK_controller_array.write_to_file(file))
			return false;
		return true;
	}
}
