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
	struct VMDMorphData
	{
		// The action corresponds to the expression name
		std::u8string	morph_name;
		// Expression deformation strength
		Float32 weight;

		explicit VMDMorphData(std::u8string morph_name = {}, const Float32& weight = {}) :
			morph_name(std::move(morph_name)), weight(weight) {}
	};

	class VMDMorphAnimation final : public VMDElement
	{
		GENERATE_MMD_CLASS_BODY(VMDMorphAnimation, VMDMorphData)
	public:
		/**
		 * \brief Default constructor function
		 */
		explicit VMDMorphAnimation() : VMDElement(), m_data(std::make_unique<data_type>()) {}
		/**
		 * \brief Constructor function
		 * \param frame_num Frame of action
		 * \param data Internal data unique_ptr
		 */
		explicit VMDMorphAnimation(const UInt32& frame_num, std::unique_ptr<data_type> data = std::make_unique<data_type>()) : VMDElement(frame_num), m_data(std::move(data)) {}
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
