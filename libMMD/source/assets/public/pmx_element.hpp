/**************************************************************************

Copyright:Copyright(c) 2022-present, Aimidi & Walter White & CMT contributors.
Author:			Aimidi
Date:			2022/10/1
File:			pmx_element.hpp
Description:	pmx element base class.

**************************************************************************/
#pragma once

#include "file_element.hpp"
#include "pmx_model_description.h"

namespace libmmd
{
	class pmx_element_impl : public file_element_impl
	{
	protected:
		const pmx_model_description* model_description_;
	public:
		/**
		 * \brief Default constructor function
		 */
		explicit pmx_element_impl(const pmx_model_description* model_description) : model_description_(model_description) {}
		/**
		 * \brief Destructor function
		 */
		~pmx_element_impl() override = default;
		/**
		 * \brief Copy constructor function
		 */
		pmx_element_impl(const pmx_element_impl&) = default;
		/**
		 * \brief Copy operator
		 * \return Self reference
		 */
		pmx_element_impl& operator =(const pmx_element_impl&) = default;
		/**
		 * \brief Move constructor function
		 */
		pmx_element_impl(pmx_element_impl&&) noexcept = default;
		/**
		 * \brief Move operator
		 * \return Self reference
		 */
		pmx_element_impl& operator =(pmx_element_impl&&) noexcept = default;
	};

	template<class T>
	// Class must base of and not same pmx_element_impl.
	concept is_pmx_element = is_base_of_and_not_same<pmx_element_impl, T>;

	template <class InterfaceClass, class ImplementationClass>
	requires
		is_file_element<ImplementationClass> &&
		is_pmx_element<ImplementationClass> && 
		is_base_of_and_not_same<InterfaceClass, ImplementationClass>
	class pmx_element_array_impl final : public pmx_element_array<InterfaceClass>
	{
		file_element_array<ImplementationClass, const pmx_model_description*> array_;
	public:
		/**
		 * \brief Default constructor function
		 */
		explicit pmx_element_array_impl(const pmx_model_description* pmx_model_info) : array_(pmx_model_info) {}
		/**
		 * \brief Destructor function
		 */
		~pmx_element_array_impl() override = default;
		/**
		 * \brief Copy constructor function
		 */
		pmx_element_array_impl(const pmx_element_array_impl&) = default;
		/**
		 * \brief Copy operator
		 * \return Self reference
		 */
		pmx_element_array_impl& operator =(const pmx_element_array_impl&) = default;
		/**
		 * \brief Move constructor function
		 */
		pmx_element_array_impl(pmx_element_array_impl&&) noexcept = default;
		/**
		 * \brief Move operator
		 * \return Self reference
		 */
		pmx_element_array_impl& operator =(pmx_element_array_impl&&) noexcept = default;


		[[nodiscard]] int32_t size() const override
		{
			return array_.size();
		}
		const InterfaceClass& operator[](int32_t index) const override
		{
			return array_[index];
		}
		InterfaceClass& operator[](int32_t index) override
		{
			return array_[index];
		}
		int32_t add(const ImplementationClass& element)
		{
			return array_.add(element);
		}
		int32_t add(ImplementationClass&& element)
		{
			return array_.add(std::move(element));
		}
		InterfaceClass& add() override
		{
			return array_.add();
		}
		ImplementationClass& add_impl()
		{
			return array_.add();
		}
		bool remove(int32_t index, uint32_t count) override
		{
			return array_.remove(index, count);
		}
		void clear()
		{
			array_.clear();
		}
		typename std::vector<InterfaceClass>::iterator begin() override
		{
			return array_.begin();
		}
		typename std::vector<InterfaceClass>::iterator end() override
		{
			return array_.end();
		}
		typename std::vector<InterfaceClass>::const_iterator begin() const override
		{
			return array_.begin();
		}
		typename std::vector<InterfaceClass>::const_iterator end() const override
		{
			return array_.end();
		}
		/**
		 * \brief Read from a pmx file
		 * \param file pmx file
		 * \return Successful TRUE, other FALSE.
		 */
		bool read_from_file(const file& file)
		{
			return array_.read_from_file(file);
		}
		/**
		 * \brief Write to pmx file
		 * \param file pmx file
		 * \return Successful TRUE, other FALSE.
		 */
		[[nodiscard]] bool write_to_file(const file& file) const
		{
			return array_.write_to_file(file);
		}
	};
}
