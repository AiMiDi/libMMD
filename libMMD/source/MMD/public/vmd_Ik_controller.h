/**************************************************************************

Copyright:Copyright(c) 2022-present, Aimidi & Walter White & CMT contributors.
Author:			Aimidi
Date:			2022/7/14
File:			vmd_Ik_controller_animation.h
Description:	MMD style IK enable information animation

**************************************************************************/

#pragma once

#include "mmd_element.hpp"

namespace libmmd
{
	struct VMDControllerData
	{
		// IK name
		std::u8string	IK_name;
		// Is IK enable
		bool	IK_enable;
		/**
		 * \brief Constructor function
		 * \param name IK name
		 * \param enable Is IK enable
		 */
		explicit VMDControllerData(std::u8string name = {}, const bool& enable = true) :
			IK_name(std::move(name)), IK_enable(enable) {}
	};

	class VMDIkController final : public MMDElement
	{
		GENERATE_MMD_CLASS_BODY(VMDIkController, VMDControllerData)
	public:
		/**
		 * \brief Default constructor function
		 */
		explicit VMDIkController() : MMDElement(), m_data(std::make_unique<data_type>()) {}
		/**
		 * \brief Constructor function
		 * \param data Internal data unique_ptr
		 */
		explicit VMDIkController(std::unique_ptr<data_type> data) : MMDElement(), m_data(std::move(data)) {}
		/**
		 * \brief Destructor function
		 */
		~VMDIkController() override = default;
		/**
		 * \brief Read from a vmd file
		 * \param file vmd file
		 * \return Successful TRUE, other FALSE
		 */
		bool read_from_file(const file& file) override;
		/**
		 * \brief Write to vmd file
		 * \param file vmd file
		 * \return Successful TRUE, other FALSE
		 */
		bool write_to_file(const file& file) const override;
	};
}
