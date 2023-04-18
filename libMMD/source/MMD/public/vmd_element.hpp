/**************************************************************************

Copyright:Copyright(c) 2022-present, Aimidi & Walter White & CMT contributors.
Author:			Aimidi
Date:			2022/7/17
File:			vmd_element.hpp
Description:	vmd element base class.

**************************************************************************/
#pragma once

#include "mmd_element.hpp"

namespace libmmd
{
	class VMDElement : public MMDElement
	{
		CMT_DISALLOW_COPY_AND_ASSIGN_BODY(VMDElement)
		CMT_DEFAULT_MOVE_BODY(VMDElement)
	protected:
		UInt32 frame_num_;
	public:
		/**
		 * \brief Default constructor function
		 */
		explicit VMDElement() : frame_num_(0U) {}
		/**
		 * \brief  Constructor function
		 */
		explicit VMDElement(const UInt32& frame_no) : frame_num_(frame_no) {}
		/**
		 * \brief Destructor function
		 */
		~VMDElement() override = default;
	public:
		[[nodiscard]] UInt32 GetFrameNum() const
		{
			return frame_num_;
		}
		/**
		 * \brief Equality operator, Sort by frame order
		 * \param other Another instance
		 * \return true is returned if it is equal to another instance, and the other is false
		 */
		bool operator ==(const VMDElement& other) const
		{
			return frame_num_ == other.frame_num_;
		}
		/**
		 * \brief Less than operator, Sort by frame order
		 * \param other Another instance
		 * \return true is returned if it is less than another instance, and the other is false
		 */
		bool operator <(const VMDElement& other) const
		{
			return frame_num_ < other.frame_num_;
		}
	};

	/**
	 * \brief VMD animation data sort array template class
	 * \tparam T VMD data element class
	 */
	template<class T>
	class VMDElementArray final : public MMDElementArray<T>
	{
		static_assert(std::is_base_of_v<VMDElement, T>);
	public:
		explicit VMDElementArray() : MMDElementArray() {}
		/**
		 * \brief Read from a vmd file
		 * \param file file vmd file
		 * \return Successful true, other false.
		 */
		bool read_from_file(const file& file) override
		{
			if (!MMDElementArray<T>::read_from_file(file))
			{
				return false;
			}
			std::sort(this->array_.begin(), this->array_.end());
			return true;
		}
		/**
		 * \brief Write to vmd file
		 * \param file file vmd file
		 * \return Successful true, other false.
		 */
		bool write_to_file(const file& file) const override
		{
			return MMDElementArray<T>::write_to_file(file);
		}
	};
}
