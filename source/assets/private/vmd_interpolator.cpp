/**************************************************************************

Copyright:Copyright(c) 2022-present, Aimidi & Walter White & CMT contributors.
Author:			Aimidi
Date:			2022/7/11
File:			vmd_interpolator.cpp
Description:	MMD style animation interpolator

**************************************************************************/

#include "vmd_interpolator.h"

namespace libmmd
{
	vmd_interpolator_impl::vmd_interpolator_impl(uint8_t ax, uint8_t ay, uint8_t bx, uint8_t by)
	{
		ax_ = ax;
		ay_ = ay;
		bx_ = bx;
		by_ = by;
		is_linear_ = ax_ == ay_ && bx_ == by_;
	}

	 bool vmd_interpolator_impl::operator==(const vmd_interpolator_impl& other) const
	{
		return ax_ == other.ax_ && ay_ == other.ay_ && bx_ == other.bx_ && by_ == other.by_;
	}

	 uint8_t vmd_interpolator_impl::get_ax() const
	{
		return ax_;
	}

	 void vmd_interpolator_impl::set_ax(uint8_t value)
	{
		ax_ = value;
		is_linear_ = ax_ == ay_ && bx_ == by_;
	}

	 uint8_t vmd_interpolator_impl::get_ay() const
	{
		return ay_;
	}

	 void vmd_interpolator_impl::set_ay(uint8_t value)
	{
		ay_ = value;
		is_linear_ = ax_ == ay_ && bx_ == by_;
	}

	 uint8_t vmd_interpolator_impl::get_bx() const
	{
		return bx_;
	}

	 void vmd_interpolator_impl::set_bx(uint8_t value)
	{
		bx_ = value;
		is_linear_ = ax_ == ay_ && bx_ == by_;
	}

	 uint8_t vmd_interpolator_impl::get_by() const
	{
		return by_;
	}

	 void vmd_interpolator_impl::set_by(uint8_t value)
	{
		by_ = value;
		is_linear_ = ax_ == ay_ && bx_ == by_;
	}

	 bool vmd_interpolator_impl::is_linear() const
	{
		return is_linear_;
	}

	 void vmd_interpolator_impl::set_linear()
	{
		ax_ = 20U;
		ay_ = 20U;
		bx_ = 107U;
		by_ = 107U;
		is_linear_ = true;
	}

	bool vmd_bone_interpolator_impl::read_from_file(const file& file)
	{
		if (!file.read_element(ax_))
			return false;
		if (!file.seek(3))
			return false;
		if (!file.read_element(ay_))
			return false;
		if (!file.seek(3))
			return false;
		if (!file.read_element(bx_))
			return false;
		if (!file.seek(3))
			return false;
		if (!file.read_element(by_))
			return false;
		if (!file.seek(3))
			return false;
		is_linear_ = ax_ == ay_ && bx_ == by_;
		return true;
	}

	bool vmd_bone_interpolator_impl::write_to_file(const file& file) const
	{
		if (!file.write_element(ax_))
			return false;
		if (!file.write_element(ax_))
			return false;
		if (!file.write_element(ax_))
			return false;
		if (!file.write_element(ax_))
			return false;
		if (!file.write_element(ay_))
			return false;
		if (!file.write_element(ay_))
			return false;
		if (!file.write_element(ay_))
			return false;
		if (!file.write_element(ay_))
			return false;
		if (!file.write_element(bx_))
			return false;
		if (!file.write_element(bx_))
			return false;
		if (!file.write_element(bx_))
			return false;
		if (!file.write_element(bx_))
			return false;
		if (!file.write_element(by_))
			return false;
		if (!file.write_element(by_))
			return false;
		if (!file.write_element(by_))
			return false;
		if (!file.write_element(by_))
			return false;
		return true;
	}

	bool vmd_camera_interpolator_impl::read_from_file(const file& file)
	{
		UInt32 tmp = 0;
		if (!file.read_element(tmp))
			return false;
		this->ax_ = static_cast<UChar>(((tmp & 0xFF) ^ 0x80) - 0x80);
		this->bx_ = static_cast<UChar>(((tmp & 0xFF00) >> 8 ^ 0x80) - 0x80);
		this->ay_ = static_cast<UChar>(((tmp & 0xFF0000) >> 16 ^ 0x80) - 0x80);
		this->by_ = static_cast<UChar>(((tmp & 0xFF000000) >> 24 ^ 0x80) - 0x80);
		is_linear_ = ax_ == ay_ && bx_ == by_;
		return true;
	}

	bool vmd_camera_interpolator_impl::write_to_file(const file& file) const
	{
		if (!file.write_element(ax_))
			return false;
		if (!file.write_element(bx_))
			return false;
		if (!file.write_element(ay_))
			return false;
		if (!file.write_element(by_))
			return false;
		return true;
	}
}
