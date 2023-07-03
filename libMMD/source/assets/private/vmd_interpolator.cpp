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
	VMDInterpolator::VMDInterpolator(const UChar ax, const UChar ay, const UChar bx, const UChar by) :
		m_ax(ax), m_ay(ay), m_bx(bx), m_by(by)
	{
		m_isLinear = m_ax == m_ay && m_bx == m_by;
	}

	VMDInterpolator::VMDInterpolator(VMDInterpolator&& src) noexcept
	{
		memmove_s(this, sizeof VMDInterpolator, &src, sizeof VMDInterpolator);
	}

	VMDInterpolator& VMDInterpolator::operator=(VMDInterpolator&& src) noexcept
	{
		if (&src == this)
		{
			return *this;
		}
		memmove_s(this, sizeof VMDInterpolator, &src, sizeof VMDInterpolator);
		return *this;
	}

	bool VMDInterpolator::operator==(const VMDInterpolator& other) const
	{
		return m_ax == other.m_ax && m_ay == other.m_ay && m_bx == other.m_bx && m_by == other.m_by;
	}

	Float64 VMDInterpolator::get_value_right() const
	{
		return static_cast<Float64>(m_ay) / 127.0;
	}

	Float64 VMDInterpolator::get_value_left() const
	{
		return static_cast<Float64>(m_by - 127) / 127.0;
	}

	Vector2d64 VMDInterpolator::get_tangent_right() const
	{
		return Vector2d64{ static_cast<Float64>(m_ax), static_cast<Float64>(m_ay) };
	}

	Vector2d64 VMDInterpolator::get_tangent_left() const
	{
		return Vector2d64{ static_cast<Float64>(m_bx - 127), static_cast<Float64>(m_by - 127) };
	}

	void VMDInterpolator::set(const UChar& ax, const UChar& ay, const UChar& bx, const UChar& by)
	{
		m_ax = ax;
		m_ay = ay;
		m_bx = bx;
		m_by = by;
		m_isLinear = m_ax == m_ay && m_bx == m_by;
	}

	void VMDInterpolator::set(const VMDInterpolator& interpolator)
	{
		m_ax = interpolator.m_ax;
		m_ay = interpolator.m_ay;
		m_bx = interpolator.m_bx;
		m_by = interpolator.m_by;
		m_isLinear = m_ax == m_ay && m_bx == m_by;
	}

	void VMDInterpolator::reset()
	{
		set();
	}

	bool VMDInterpolator::is_linear() const
	{
		return m_isLinear;
	}

	bool VMDBoneInterpolator::read(const file& file)
	{
		if (!file.read_elements(m_ax))
			return false;
		if (!file.seek(3))
			return false;
		if (!file.read_elements(m_ay))
			return false;
		if (!file.seek(3))
			return false;
		if (!file.read_elements(m_bx))
			return false;
		if (!file.seek(3))
			return false;
		if (!file.read_elements(m_by))
			return false;
		if (!file.seek(3))
			return false;
		m_isLinear = m_ax == m_ay && m_bx == m_by;
		return true;
	}

	bool VMDBoneInterpolator::write(const file& file) const
	{
		if (!file.write_elements(m_ax))
			return false;
		if (!file.write_elements(m_ax))
			return false;
		if (!file.write_elements(m_ax))
			return false;
		if (!file.write_elements(m_ax))
			return false;
		if (!file.write_elements(m_ay))
			return false;
		if (!file.write_elements(m_ay))
			return false;
		if (!file.write_elements(m_ay))
			return false;
		if (!file.write_elements(m_ay))
			return false;
		if (!file.write_elements(m_bx))
			return false;
		if (!file.write_elements(m_bx))
			return false;
		if (!file.write_elements(m_bx))
			return false;
		if (!file.write_elements(m_bx))
			return false;
		if (!file.write_elements(m_by))
			return false;
		if (!file.write_elements(m_by))
			return false;
		if (!file.write_elements(m_by))
			return false;
		if (!file.write_elements(m_by))
			return false;
		return true;
	}

	bool VMDCameraInterpolator::read(const file& file)
	{
		UInt32 tmp = 0;
		if (!file.read_elements(tmp))
			return false;
		this->m_ax = static_cast<UChar>(((tmp & 0xFF) ^ 0x80) - 0x80);
		this->m_bx = static_cast<UChar>(((tmp & 0xFF00) >> 8 ^ 0x80) - 0x80);
		this->m_ay = static_cast<UChar>(((tmp & 0xFF0000) >> 16 ^ 0x80) - 0x80);
		this->m_by = static_cast<UChar>(((tmp & 0xFF000000) >> 24 ^ 0x80) - 0x80);
		m_isLinear = m_ax == m_ay && m_bx == m_by;
		return true;
	}

	bool VMDCameraInterpolator::write(const file& file) const
	{
		if (!file.write_elements(m_ax))
			return false;
		if (!file.write_elements(m_bx))
			return false;
		if (!file.write_elements(m_ay))
			return false;
		if (!file.write_elements(m_by))
			return false;
		return true;
	}
}