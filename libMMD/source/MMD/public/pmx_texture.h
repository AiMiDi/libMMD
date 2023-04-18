/**************************************************************************

Copyright:Copyright(c) 2022-present, Aimidi & Walter White & CMT contributors.
Author:			Aimidi
Date:			2022/10/2
File:			pmx_texture.h
Description:	pmx texture data

**************************************************************************/

#pragma once

#include "pmx_element.hpp"

namespace libmmd
{
	struct PMXTextureData
	{
		std::u8string path;

		explicit PMXTextureData(std::u8string path_ = {}) : path(std::move(path_)) {}
	};

	class PMXTexture final : public PMXElement
	{
		GENERATE_MMD_CLASS_BODY(PMXTexture, PMXTextureData)
	public:
		/**
		 * \brief Default constructor function
		 */
		explicit PMXTexture(const PMXModelInfo* model_info) : PMXElement(model_info), m_data(std::make_unique<data_type>()) {}
		/**
		 * \brief Destructor function
		 */
		~PMXTexture() override = default;
		/**
		 * \brief Read PMX texture data from a pmx file
		 * \param file file pmx file
		 * \return Successful TRUE, other FALSE.
		 */
		bool read_from_file(const file& file) override;
	};
}
