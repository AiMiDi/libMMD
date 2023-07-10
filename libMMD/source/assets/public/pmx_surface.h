/**************************************************************************

Copyright:Copyright(c) 2022-present, Aimidi & Walter White & CMT contributors.
Author:			Aimidi
Date:			2023/6/4
File:			pmx_surface.h
Description:	pmx surface data

**************************************************************************/

#pragma once
#include "pmx_element.hpp"
#include "pmx_model_description.h"

namespace libmmd
{
	class pmx_surface_impl final : public pmx_element_impl, public pmx_surface
	{
		UInt32 index_[3];
	public:
		/**
		 * \brief Default constructor function
		 */
		explicit pmx_surface_impl(const pmx_model_description* model_info, const UInt32 a = 0, const UInt32 b = 1, const UInt32 c = 2) :
			pmx_element_impl(model_info), index_{ a, b, c } {}
		/**
		 * \brief Destructor function
		 */
		~pmx_surface_impl() override = default;
		pmx_surface_impl(const pmx_surface_impl&) = default;
		pmx_surface_impl& operator =(const pmx_surface_impl&) = default;
		pmx_surface_impl(pmx_surface_impl&&) noexcept = default;
		pmx_surface_impl& operator =(pmx_surface_impl&&) noexcept = default;

		[[nodiscard]] const uint32_t& get_a() const override;
		void set_a(const uint32_t& point_index) override;

		[[nodiscard]] const uint32_t& get_b() const override;
		void set_b(const uint32_t& point_index) override;

		[[nodiscard]] const uint32_t& get_c() const override;
		void set_c(const uint32_t& point_index) override;

		/**
		 * \brief Read PMX polygon data from a pmx file
		 * \param file file pmx file
		 * \return Successful TRUE, other FALSE.
		 */
		bool read_from_file(const file& file) override;
		/**
		 * \brief Write PMX vertex data to a pmx file
		 * \param file pmx file
		 * \return Successful TRUE, other FALSE.
		 */
		bool write_to_file(const file& file) const override;
	};
}
