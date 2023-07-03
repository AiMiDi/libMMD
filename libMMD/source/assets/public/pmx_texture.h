/**************************************************************************

Copyright:Copyright(c) 2022-present, Aimidi & Walter White & CMT contributors.
Author:			Aimidi
Date:			2022/10/2
File:			pmx_texture.h
Description:	pmx texture data

**************************************************************************/

#pragma once
#include "pmx_model_info.h"
#include "pmx_element_array.hpp"

namespace libmmd
{
	class pmx_texture_impl final : public pmx_element_impl, pmx_texture
	{
		std::u8string path_{};

	public:
		/**
		 * \brief Default constructor function
		 */
		explicit pmx_texture_impl(const pmx_model_description* model_info) : pmx_element_impl(model_info){}
		/**
		 * \brief Destructor function
		 */
		~pmx_texture_impl() override = default;

		pmx_texture_impl(const pmx_texture_impl&) = default;
		pmx_texture_impl& operator =(const pmx_texture_impl&) = default;
		pmx_texture_impl(pmx_texture_impl&&) noexcept = default;
		pmx_texture_impl& operator =(pmx_texture_impl&&) noexcept = default;

		[[nodiscard]] std::string get_texture_path() const override
		{
			return std::string{path_.begin(), path_.end()};
		}
		void set_texture_path(std::string& path) override
		{
			path_ = std::u8string{ path.begin(), path.end() };
		}

		/**
		 * \brief Read PMX vertex data from a pmx file
		 * \param file pmx file
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
