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
	class VMDIkController final : public mmd_element
	{
		// IK name
		std::u8string	name_{};
		// Is IK enable
		bool	enable_{};

		CMT_DEFAULT_COPY_BODY(VMDIkController)
		CMT_DEFAULT_MOVE_BODY(VMDIkController)
	public:
		/**
		 * \brief Constructor function
		 * \param name IK name
		 * \param enable Is IK enable
		 */
		explicit VMDIkController(
			std::u8string name = {}, 
			const bool& enable = true) :
			name_(std::move(name)),
			enable_(enable) {}
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
