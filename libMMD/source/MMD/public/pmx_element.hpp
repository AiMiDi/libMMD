/**************************************************************************

Copyright:Copyright(c) 2022-present, Aimidi & Walter White & CMT contributors.
Author:			Aimidi
Date:			2022/10/1
File:			pmx_element.hpp
Description:	pmx element base class.

**************************************************************************/
#pragma once

#include "mmd_element.hpp"
#include "pmx_model_info.h"

namespace libmmd
{

	class PMXElement : public MMDElement
	{
		CMT_DISALLOW_COPY_AND_ASSIGN_BODY(PMXElement)
	protected:
		const PMXModelInfo* model_info_ = nullptr;
	public:
		/**
		* \brief  Constructor function
		*/
		explicit PMXElement(const PMXModelInfo* model_info) : model_info_(model_info) {}
		/**
		 * \brief Move constructor
		 */
		PMXElement(PMXElement&& src) noexcept : model_info_(src.model_info_) {}
		/**
		 * \brief Move operator=
		 * \return Result reference
		 */
		PMXElement& operator =(PMXElement&& src) noexcept
		{
			if (this != &src)
				model_info_ = src.model_info_;
			return *this;
		}
		/**
		 * \brief Destructor function
		 */
		~PMXElement() override = default;
	};

	/**
	 * \brief PMX data array template class
	 * \tparam T PMX data element class
	 */
	template<class T>
	class PMXElementArray final : public MMDElementArray<T, const PMXModelInfo*>
	{
		static_assert(std::is_base_of_v<PMXElement, T>);
	public:
		explicit PMXElementArray(const PMXModelInfo* model_info_ptr) :
			MMDElementArray<T, const PMXModelInfo*>(model_info_ptr) {}
		/**
		 * \brief Read from a pmx file
		 * \param file file pmx file
		 * \return Successful TRUE, other FALSE.
		 */
		bool read_from_file(const file& file) override
		{
			if (!MMDElementArray<T, const PMXModelInfo*>::read_from_file(file))
			{
				return false;
			}
			return true;
		}
		/**
		 * \brief Write to pmx file
		 * \param file file pmx file
		 * \return Successful TRUE, other FALSE.
		 */
		bool write_to_file(const file& file) const override
		{
			return MMDElementArray<T, const PMXModelInfo*>::write_to_file(file);
		}
	};
}
