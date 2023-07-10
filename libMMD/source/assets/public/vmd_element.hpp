/**************************************************************************

Copyright:Copyright(c) 2023-present, Aimidi libmmd contributors.
Author:			Aimidi
Date:			2023/7/
File:			vmd_element.hpp
Description:	vmd element base class.

**************************************************************************/
#pragma once

#include "file_element.hpp"

namespace libmmd
{
	class vmd_key_frame_impl : virtual public vmd_key_frame, public file_element_impl
	{
	protected:
		uint32_t frame_at_ = 0;
	public:
		/**
		 * \brief  Constructor function
		 */
		vmd_key_frame_impl() = default;
		/**
		 * \brief Destructor function
		 */
		~vmd_key_frame_impl() override = default;
		/**
		 * \brief Copy constructor
		 */
		vmd_key_frame_impl(const vmd_key_frame_impl&) noexcept = default;
		/**
		 * \brief Move constructor
		 */
		vmd_key_frame_impl(vmd_key_frame_impl&&) noexcept = default;
		/**
		* \brief Copy operator=
		* \return Result reference
		*/
		vmd_key_frame_impl& operator =(const vmd_key_frame_impl&) = default;
		/**
		 * \brief Move operator=
		 * \return Result reference
		 */
		vmd_key_frame_impl& operator =(vmd_key_frame_impl&&) noexcept = default;

		[[nodiscard]] uint32_t get_frame_at() const override
		{
			return frame_at_;
		}
		void set_frame_at(const uint32_t frame_at) override
		{
			frame_at_ = frame_at;
		}

		bool operator==(const vmd_key_frame_impl& other) const
		{
			return frame_at_ == other.frame_at_;
		}

		bool operator<(const vmd_key_frame_impl& other) const
		{
			return frame_at_ < other.frame_at_;
		}
	};

	template<typename T>
	// Class must base of and not same vmd_key_frame_impl.
	concept is_vmd_element = is_base_of_and_not_same<vmd_key_frame_impl, T>;

	template<typename T>
	// Class must have < and == operator
	concept have_compare = requires(T a, T b)
	{
		a == b;
		a < b;
	};

	template <class InterfaceClass, class ImplementationClass>
		requires
		(is_file_element<ImplementationClass>&&
		is_vmd_element<ImplementationClass>&&
		is_base_of_and_not_same<InterfaceClass, ImplementationClass>) ||
		have_compare<ImplementationClass>
	class vmd_element_array_impl final : public vmd_element_array<InterfaceClass>
	{
		file_element_array<ImplementationClass> array_;
	public:
		/**
		 * \brief Default constructor function
		 */
		explicit vmd_element_array_impl() = default;
		/**
		 * \brief Destructor function
		 */
		~vmd_element_array_impl() override = default;
		/**
		 * \brief Copy constructor function
		 */
		vmd_element_array_impl(const vmd_element_array_impl&) = default;
		/**
		 * \brief Copy operator
		 * \return Self reference
		 */
		vmd_element_array_impl& operator =(const vmd_element_array_impl&) = default;
		/**
		 * \brief Move constructor function
		 */
		vmd_element_array_impl(vmd_element_array_impl&&) noexcept = default;
		/**
		 * \brief Move operator
		 * \return Self reference
		 */
		vmd_element_array_impl& operator =(vmd_element_array_impl&&) noexcept = default;


		[[nodiscard]] uint64_t size() const override
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
		auto begin()
		{
			return array_.begin();
		}
		auto end()
		{
			return array_.end();
		}
		auto begin() const
		{
			return array_.begin();
		}
		auto end() const
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

		void sort() override
		{
			std::sort(array_.begin(), array_.end());
		}
	};

	
}
