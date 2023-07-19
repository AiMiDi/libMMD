/**************************************************************************

Copyright:Copyright(c) 2022-present, Aimidi & Walter White & CMT contributors.
Author:			Aimidi
Date:			2022/7/11
File:			vmd_interpolator.h
Description:	MMD style animation interpolator

**************************************************************************/
#pragma once

#include "file_element.hpp"

namespace libmmd
{
	/**
	 * \brief MMD style animation interpolator
	 */
	class vmd_interpolator_impl : public vmd_interpolator, file_element_impl
	{
	protected:
		uint8_t ax_ = 20U;
		uint8_t ay_ = 20U;
		uint8_t bx_ = 107U;
		uint8_t by_ = 107U;
		bool  is_linear_ = true;
	public:
		/**
		 * \brief  Constructor function
		 */
		explicit vmd_interpolator_impl(uint8_t ax = 20U, uint8_t ay = 20U, uint8_t bx = 107U, uint8_t by = 107U);
		/**
		 * \brief Destructor function
		 */
		~vmd_interpolator_impl() override = default;
		/**
		 * \brief Copy constructor
		 */
		vmd_interpolator_impl(const vmd_interpolator_impl&) noexcept = default;
		/**
		 * \brief Move constructor
		 */
		vmd_interpolator_impl(vmd_interpolator_impl&&) noexcept = default;
		/**
		* \brief Copy operator=
		* \return Result reference
		*/
		vmd_interpolator_impl& operator =(const vmd_interpolator_impl&) = default;
		/**
		 * \brief Move operator=
		 * \return Result reference
		 */
		vmd_interpolator_impl& operator =(vmd_interpolator_impl&&) noexcept = default;

		bool operator==(const vmd_interpolator_impl& other) const;

		uint8_t get_ax() const override;
		void set_ax(uint8_t value) override;

		uint8_t get_ay() const override;
		void set_ay(uint8_t value) override;

		uint8_t get_bx() const override;
		void set_bx(uint8_t value) override;

		uint8_t get_by() const override;
		void set_by(uint8_t value) override;

		bool is_linear() const override;
		void set_linear() override;
	};

	class vmd_bone_interpolator_impl final : public vmd_interpolator_impl
	{
	public:
		bool read_from_file(const file& file) override;
		[[nodiscard]] bool write_to_file(const file& file) const override;
	};

	class vmd_camera_interpolator_impl final : public vmd_interpolator_impl
	{
	public:
		bool read_from_file(const file& file) override;
		[[nodiscard]] bool write_to_file(const file& file) const override;
	};
}

template<>
struct std::hash<libmmd::vmd_interpolator_impl>
{
	size_t operator()(const libmmd::vmd_interpolator_impl& rhs) const noexcept
	{
		return std::hash<int>()(rhs.get_ax()) ^ std::hash<int>()(rhs.get_ay()) ^ std::hash<int>()(rhs.get_bx()) ^ std::hash<int>()(rhs.get_by()) << 1;
	}
};
