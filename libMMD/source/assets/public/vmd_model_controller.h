/**************************************************************************

Copyright:Copyright(c) 2022-present, Aimidi & Walter White & CMT contributors.
Author:			Aimidi
Date:			2022/7/14
File:			vmd_model_controller_animation.h
Description:	MMD style model information animation

**************************************************************************/

#pragma once

#include "vmd_element.hpp"
#include "vmd_Ik_controller.h"

namespace libmmd
{
	class VMDModelController final : public vmd_element
	{
	public:
		typedef file_element_array<VMDIkController> VMDIkControllerArray;
	private:
		// Is model show
		bool				 show_;
		// IKs enable information
		VMDIkControllerArray IK_controller_animation_{};

		CMT_DEFAULT_COPY_BODY(VMDModelController)
		CMT_DEFAULT_MOVE_BODY(VMDModelController)
	public:
		/**
		 * \brief  Constructor function
		 * \param show Is model show
		 */
		explicit VMDModelController(const bool& show = true) : show_(show) {}
		/**
		 * \brief Destructor function
		 */
		~VMDModelController() override = default;
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
