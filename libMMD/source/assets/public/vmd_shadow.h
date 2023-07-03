/**************************************************************************

Copyright:Copyright(c) 2022-present, Aimidi & Walter White & CMT contributors.
Author:			Aimidi
Date:			2022/7/15
File:			vmd_shadow_animation.h
Description:	MMD style shadow animation

**************************************************************************/

#pragma once

#include "vmd_element.hpp"

namespace libmmd
{
	class VMDShadow final : public vmd_element
	{
		// 0:Off 1:mode1 2:mode2
		UChar	shadow_type_{};
		// Distance
		Float32 distance_{};

		CMT_DEFAULT_COPY_BODY(VMDShadow)
		CMT_DEFAULT_MOVE_BODY(VMDShadow)
	public:
		/**
		 * \brief Constructor function
		 * \param shadow_type 0:Off 1:mode1 2:mode2
		 * \param distance Shadow distance
		 */
		explicit VMDShadow(
			const UChar& shadow_type = {}, 
			const Float32& distance = {}) :
			shadow_type_(shadow_type),
			distance_(distance) {}
		/**
		 * \brief Destructor function
		 */
		~VMDShadow() override = default;
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
