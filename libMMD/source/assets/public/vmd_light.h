/**************************************************************************

Copyright:Copyright(c) 2022-present, Aimidi & Walter White & CMT contributors.
Author:			Aimidi
Date:			2022/7/15
File:			vmd_light_animation.h
Description:	MMD style lighting animation

**************************************************************************/

#pragma once

#include "vmd_element.hpp"

namespace libmmd
{
	class VMDLightAnimation final : public vmd_element
	{
		// Light color
		Vector32	light_color_{};
		// location
		Vector32	light_position_{};

		CMT_DEFAULT_COPY_BODY(VMDLightAnimation)
		CMT_DEFAULT_MOVE_BODY(VMDLightAnimation)
	public:
		/**
		 * \brief Constructor function
		 * \param light_color Light color
		 * \param light_position Location
		 */
		explicit VMDLightAnimation(
			const Vector32& light_color = {}, 
			const Vector32& light_position = {}) :
			light_color_(light_color),
			light_position_(light_position) {}
		/**
		 * \brief Destructor function
		 */
		~VMDLightAnimation() override = default;
		/**
		 * \brief Read from a vmd file
		 * \param file vmd file
		 * \return Successful TRUE, other FALSE.
		 */
		bool read_from_file(const file& file) override;
		/**
		 * \brief Write to vmd file
		 * \param file vmd file
		 * \return Successful TRUE, other FALSE.
		 */
		bool write_to_file(const file& file) const override;
	};
}
