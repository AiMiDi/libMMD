/**************************************************************************

Copyright:Copyright(c) 2022-present, Aimidi & Walter White & CMT contributors.
Author:			Aimidi
Date:			2022/7/16
File:			vmd_camera_animation.cpp
Description:	

**************************************************************************/

#include "vmd_camera.h"

namespace libmmd
{
	inline float vmd_camera_key_frame_impl::get_distance() const
	{
		return distance_;
	}

	inline void vmd_camera_key_frame_impl::set_distance(const float distance)
	{
		distance_ = distance;
	}

	inline const std::array<float, 3>& vmd_camera_key_frame_impl::get_position() const
	{
		return position_;
	}

	inline void vmd_camera_key_frame_impl::set_position(const std::array<float, 3>& position)
	{
		position_ = position;
	}

	inline const std::array<float, 3>& vmd_camera_key_frame_impl::get_rotation() const
	{
		return rotation_;
	}

	inline void vmd_camera_key_frame_impl::set_rotation(const std::array<float, 3>& rotation)
	{
		rotation_ = rotation;
	}

	inline uint32_t vmd_camera_key_frame_impl::get_view_angle() const
	{
		return viewing_angle_;
	}

	inline void vmd_camera_key_frame_impl::set_view_angle(const uint32_t view_angle)
	{
		viewing_angle_ = view_angle;
	}

	inline bool vmd_camera_key_frame_impl::is_perspective() const
	{
		return perspective_;
	}

	inline void vmd_camera_key_frame_impl::set_perspective(bool value)
	{
		perspective_ = value;
	}

	inline const vmd_interpolator& vmd_camera_key_frame_impl::get_position_x_interpolator() const
	{
		return interpolator_position_x_;
	}

	inline vmd_interpolator& vmd_camera_key_frame_impl::mutable_position_x_interpolator()
	{
		return interpolator_position_x_;
	}

	inline const vmd_interpolator& vmd_camera_key_frame_impl::get_position_y_interpolator() const
	{
		return interpolator_position_y_;
	}

	inline vmd_interpolator& vmd_camera_key_frame_impl::mutable_position_y_interpolator()
	{
		return interpolator_position_y_;
	}

	inline const vmd_interpolator& vmd_camera_key_frame_impl::get_position_z_interpolator() const
	{
		return interpolator_position_z_;
	}

	inline vmd_interpolator& vmd_camera_key_frame_impl::mutable_position_z_interpolator()
	{
		return interpolator_position_z_;
	}

	inline const vmd_interpolator& vmd_camera_key_frame_impl::get_rotation_interpolator() const
	{
		return interpolator_rotation_;
	}

	inline vmd_interpolator& vmd_camera_key_frame_impl::mutable_rotation_interpolator()
	{
		return interpolator_rotation_;
	}

	inline const vmd_interpolator& vmd_camera_key_frame_impl::get_distance_interpolator() const
	{
		return interpolator_distance_;
	}

	inline vmd_interpolator& vmd_camera_key_frame_impl::mutable_distance_interpolator()
	{
		return interpolator_distance_;
	}

	const vmd_interpolator& vmd_camera_key_frame_impl::get_view_angle_interpolator() const
	{
		return interpolator_viewing_angle_;
	}

	inline vmd_interpolator& vmd_camera_key_frame_impl::mutable_view_angle_interpolator()
	{
		return interpolator_viewing_angle_;
	}

	bool vmd_camera_key_frame_impl::read_from_file(const file& file)
	{
		if (!file.read_elements(frame_at_))
			return false;
		if (!file.read_elements(distance_))
			return false;
		if (!file.read_elements(position_))
			return false;
		if (!file.read_elements(rotation_))
			return false;
		if (!interpolator_position_x_.read_from_file(file))
			return false;
		if (!interpolator_position_y_.read_from_file(file))
			return false;
		if (!interpolator_position_z_.read_from_file(file))
			return false;
		if (!interpolator_rotation_.read_from_file(file))
			return false;
		if (!interpolator_distance_.read_from_file(file))
			return false;
		if (!interpolator_viewing_angle_.read_from_file(file))
			return false;
		if (!file.read_elements(viewing_angle_))
			return false;
		if (!file.read_elements(perspective_))
			return false;
		return true;
	}

	bool vmd_camera_key_frame_impl::write_to_file(const file& file) const
	{
		if (!file.write_elements(frame_at_))
			return false;
		if (!file.write_elements(distance_))
			return false;
		if (!file.write_elements(position_))
			return false;
		if (!file.write_elements(rotation_))
			return false;
		if (!interpolator_position_x_.write_to_file(file))
			return false;
		if (!interpolator_position_y_.write_to_file(file))
			return false;
		if (!interpolator_position_z_.write_to_file(file))
			return false;
		if (!interpolator_rotation_.write_to_file(file))
			return false;
		if (!interpolator_distance_.write_to_file(file))
			return false;
		if (!interpolator_viewing_angle_.write_to_file(file))
			return false;
		if (!file.write_elements(viewing_angle_))
			return false;
		if (!file.write_elements(perspective_))
			return false;
		return true;
	}
}
