/**************************************************************************

Copyright:Copyright(c) 2022-present, Aimidi & Walter White & CMT contributors.
Author:			Aimidi
Date:			2023/6/14
File:			file_element_array.hpp
Description:	MMD file element array class.

**************************************************************************/
#pragma once

namespace libmmd
{
	class file_element_impl
	{
	public:
		/**
		 * \brief Default constructor function
		 */
		file_element_impl() = default;
		/**
		 * \brief Destructor function
		 */
		virtual ~file_element_impl() = default;
		/**
		 * \brief Copy constructor function
		 */
		file_element_impl(const file_element_impl&) = default;
		/**
		 * \brief Copy operator
		 * \return Self reference
		 */
		file_element_impl& operator =(const file_element_impl&) = default;
		/**
		 * \brief Move constructor function
		 */
		file_element_impl(file_element_impl&&) noexcept = default;
		/**
		 * \brief Move operator
		 * \return Self reference
		 */
		file_element_impl& operator =(file_element_impl&&) noexcept = default;
		/**
		 * \brief Read file element data from a pmx file
		 * \param file file
		 * \return Successful TRUE, other FALSE.
		 */
		virtual bool read_from_file(const file& file) = 0;
		/**
		 * \brief Write file element data to a pmx file
		 * \param file file
		 * \return Successful TRUE, other FALSE.
		 */
		[[nodiscard]] virtual bool write_to_file(const file& file) const = 0;
	};

	template<class Base, class Derived>
	// Base is not same Derived and Base is base of Derived
	concept is_base_of_and_not_same = !std::is_same_v<Base, Derived>&& std::is_base_of_v<Base, Derived>;

	template<typename T>
	// T must have read_from_file() and write_to_file()
	concept is_file_element = is_base_of_and_not_same<file_element_impl, T>;

	template<typename T, typename... Args> requires is_file_element<T>
	class file_element_array final
	{
		using file_element_type = T;
		Int32 data_number_coefficient_ = 0;
		std::tuple<Args...> default_construction_args_;
	protected:
		std::vector<file_element_type> data_;
	public:
		explicit file_element_array(Args... args) : default_construction_args_(std::forward<Args>(args)...) {}
		~file_element_array() = default;
		file_element_array(const file_element_array&) = default;
		file_element_array& operator =(const file_element_array&) = default;
		file_element_array(file_element_array&&) = default;
		file_element_array& operator=(file_element_array&&) = default;

		/**
		 * \brieff Set data number coefficient
		 * \param coefficient The coefficient
		 */
		void set_data_number_coefficient(const Int32 coefficient)
		{
			data_number_coefficient_ = coefficient;
		}

		/**
		 * \brief Read from a file
		 * \param file The file
		 * \return Successful TRUE, other FALSE.
		 */
		bool read_from_file(const file& file)
		{
			auto data_number = Int32();
			if (!file.read_element(data_number))
				return false;
			data_number /= data_number_coefficient_;
			data_.resize(data_number, std::make_from_tuple<file_element_type>(default_construction_args_));
			for (auto data_index = decltype(data_number){}; data_index < data_number; ++data_index)
			{
				if (!data_[data_index].read_from_file(file))
				{
					return false;
				}
			}
			return true;
		}
		/**
		 * \brief Write to file
		 * \param file The file
		 * \return Successful TRUE, other FALSE.
		 */
		bool write_to_file(const file& file) const
		{
			const auto data_number = static_cast<Int32>(data_.size()) * data_number_coefficient_;
			if (!file.write_element(data_number))
				return false;
			for (auto data_index = decltype(data_number){}; data_index < data_number; ++data_index)
			{
				if (!data_[data_index].write_to_file(file))
				{
					return false;
				}
			}
			return true;
		}

		uint64_t size() const
		{
			return data_.size();
		}
		const file_element_type& operator[](uint64_t index) const
		{
			return data_[index];
		}
		file_element_type& operator[](uint64_t index)
		{
			return data_[index];
		}
		uint64_t add(const file_element_type& element)
		{
			data_.push_back(element);
			return data_.size() - 1ULL;
		}
		uint64_t add(file_element_type&& element)
		{
			data_.push_back(std::move(element));
			return data_.size() - 1ULL;
		}
		file_element_type& add()
		{
			data_.push_back(std::make_from_tuple<file_element_type>(default_construction_args_));
			return data_[data_.size() - 1];
		}

		bool remove(uint64_t index, uint64_t count)
		{
			if (index + count > data_.size())
				return false;

			auto begin_iterator = data_.begin() + index;
			data_.erase(begin_iterator, begin_iterator + count);

			return true;
		}

		auto begin()
		{
			return data_.begin();
		}

		auto end()
		{
			return data_.end();
		}

		auto begin() const
		{
			return data_.begin();
		}

		auto end() const
		{
			return data_.end();
		}

		void clear()
		{
			data_.clear();
		}
	};
}
