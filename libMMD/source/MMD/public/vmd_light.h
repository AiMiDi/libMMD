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
	struct VMDLightData
	{
		// Light color
		Vector32	light_color{};
		// location
		Vector32	light_position{};
		/**
		 * \brief Constructor function
		 * \param light_color Light color
		 * \param light_position Location
		 */
		explicit VMDLightData(Vector32 light_color = {}, Vector32 light_position = {}) :
			light_color(std::move(light_color)), light_position(std::move(light_position)) {}
	};

	class VMDLightAnimation final : public VMDElement
	{
		GENERATE_MMD_CLASS_BODY(VMDLightAnimation, VMDLightData)
	public:
		/**
		 * \brief Default constructor function
		 */
		explicit VMDLightAnimation() : VMDElement(), m_data(std::make_unique<data_type>()) {}
		/**
		 * \brief Constructor function
		 * \param frame_num Frame of action
		 * \param data Internal data unique_ptr
		 */
		explicit VMDLightAnimation(const UInt32& frame_num, std::unique_ptr<data_type> data = std::make_unique<data_type>()) : VMDElement(frame_num), m_data(std::move(data)) {}
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
