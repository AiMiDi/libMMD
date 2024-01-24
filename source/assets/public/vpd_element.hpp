#pragma once
#include "file_element.hpp"

namespace libmmd
{
	template <class InterfaceClass, class ImplementationClass>
		requires
		is_base_of_and_not_same<InterfaceClass, ImplementationClass>
	class vpd_element_array_impl final : public vpd_element_array<InterfaceClass>
	{
		std::vector<ImplementationClass> array_;
	public:
		/**
		 * \brief Default constructor function
		 */
		vpd_element_array_impl() = default;
		/**
		 * \brief Destructor function
		 */
		~vpd_element_array_impl() override = default;
		/**
		 * \brief Copy constructor function
		 */
		vpd_element_array_impl(const vpd_element_array_impl&) = default;
		/**
		 * \brief Copy operator
		 * \return Self reference
		 */
		vpd_element_array_impl& operator =(const vpd_element_array_impl&) = default;
		/**
		 * \brief Move constructor function
		 */
		vpd_element_array_impl(vpd_element_array_impl&&) noexcept = default;
		/**
		 * \brief Move operator
		 * \return Self reference
		 */
		vpd_element_array_impl& operator =(vpd_element_array_impl&&) noexcept = default;


		[[nodiscard]] uint64_t size() const override
		{
			return array_.size();
		}
		const InterfaceClass& operator[](uint64_t index) const override
		{
			return array_[index];
		}
		InterfaceClass& operator[](uint64_t index) override
		{
			return array_[index];
		}
		InterfaceClass& add() override
		{
			array_.push_back(ImplementationClass{});
			return array_.back();
		}
		uint64_t add_impl(const ImplementationClass& element)
		{
			array_.push_back(element);
			return array_.size();
		}
		uint64_t add_impl(ImplementationClass&& element)
		{
			array_.push_back(std::move(element));
			return array_.size();
		}
		ImplementationClass& add_impl()
		{
			array_.push_back(ImplementationClass{});
			return array_.back();
		}
		bool remove(uint64_t index, uint64_t count) override
		{
			if (index + count > array_.size())
				return false;

			auto begin_iterator = array_.begin() + index;
			array_.erase(begin_iterator, begin_iterator + count);

			return true;
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
	};
}
