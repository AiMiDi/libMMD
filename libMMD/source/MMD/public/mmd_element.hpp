/**************************************************************************

Copyright:Copyright(c) 2022-present, Aimidi & Walter White & CMT contributors.
Author:			Aimidi
Date:			2022/8/13
File:			mmd_element.hpp
Description:	MMD element base class.

**************************************************************************/
#pragma once

namespace libmmd
{
	class MMDElement
	{
	public:
		MMDElement() = default;
		virtual  ~MMDElement() = default;
		CMT_DISALLOW_COPY_AND_ASSIGN_BODY(MMDElement)
		/**
		 * \brief Move constructor
		 */
		MMDElement(MMDElement&&) noexcept = default;
		/**
		 * \brief Move operator=
		 * \return Result reference
		 */
		MMDElement& operator =(MMDElement&&) noexcept = default;
		/**
		* \brief Read from a mmd data file(pmx/vmd and more)
		* \param file mmd data file(pmx/vmd and more)
		* \return Successful TRUE, other FALSE.
		*/
		virtual bool read_from_file(const file& file)
		{
			//DebugStop("Not overloaded MMDElement::ReadFromFile()"_s);
			return false;
		}
		/**
		 * \brief Write to mmd data file(pmx/vmd and more)
		 * \param file mmd data file(pmx/vmd and more)
		 * \return Successful TRUE, other FALSE.
		 */
		virtual bool write_to_file(const file& file) const
		{
			//DebugStop("Not overloaded MMDElement::WriteToFile()"_s);
			return false;
		}
	};

	/**
	 * \brief MMD data array template class
	 * \tparam T MMD data element class
	 */
	template<class T, typename... Args>
	class MMDElementArray
	{
		static_assert(std::is_base_of_v<MMDElement, T>);
		std::tuple<Args...> args_;
	protected:
		std::vector<T> array_;
	public:
		explicit MMDElementArray(Args... args) : args_(std::forward<Args>(args)...) {}
		virtual ~MMDElementArray() = default;

		/**
		 * \brief Read from a mmd file
		 * \param file file mmd file
		 * \return Successful TRUE, other FALSE.
		 */
		virtual bool read_from_file(const file& file)
		{
			uint32_t data_number = 0;
			if (!file.read_elements(data_number))
				return false;
			array_.clear();
			array_.reserve(data_number);
			for (int data_index = 0; data_index < data_number; ++data_index)
			{
				T data = std::make_from_tuple<T>(args_);
				if (!data.read_from_file(file))
				{
					return false;
				}
				array_.push_back(std::move(data));
			}
			return true;
		}
		/**
		 * \brief Write to mmd file
		 * \param file file mmd file
		 * \return Successful TRUE, other FALSE.
		 */
		virtual bool write_to_file(const file& file) const
		{
			if (const auto data_number = static_cast<UInt32>(array_.size()); !file.write_elements(data_number))
				return false;
			for (auto& data : *this)
			{
				if (!data.write_to_file(file))
				{
					return false;
				}
			}
			return true;
		}
	};
}
