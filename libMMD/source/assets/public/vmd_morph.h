/**************************************************************************

Copyright:Copyright(c) 2022-present, Aimidi & Walter White & CMT contributors.
Author:			Aimidi
Date:			2022/7/15
File:			vmd_morph_animation.h
Description:	MMD style expression animation

**************************************************************************/

#pragma once

#include "vmd_element.hpp"

namespace libmmd
{
	class VMDMorphAnimation final : public vmd_element
	{
		// The action corresponds to the expression name
		std::u8string	morph_name_{};
		// Expression deformation strength
		Float32 weight_{};

		CMT_DEFAULT_COPY_BODY(VMDMorphAnimation)
		CMT_DEFAULT_MOVE_BODY(VMDMorphAnimation)
	public:
		/**
		 * \brief Constructor function
		 * \param morph_name morph name
		 * \param weight morph weight
		 */
		explicit VMDMorphAnimation(
			std::u8string morph_name = {},
			const Float32& weight = {}) :
			morph_name_(std::move(morph_name)),
			weight_(weight) {}
		/**
		 * \brief Destructor function
		 */
		~VMDMorphAnimation() override = default;
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
