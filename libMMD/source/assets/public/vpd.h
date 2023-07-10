/**************************************************************************

Copyright:Copyright(c) 2022-present, Aimidi & Walter White & CMT contributors.
Author:			Aimidi
Date:			2022/7/2
File:			vpd_data.h
Description:	vpd file data

**************************************************************************/

#pragma once
#include "file_element.hpp"

namespace libmmd
{
	template <class InterfaceClass, class ImplementationClass>
		requires
		is_base_of_and_not_same<InterfaceClass, ImplementationClass>
		class vpd_element_array_impl final : public vpd_element_array<InterfaceClass>
	{
		file_element_array<ImplementationClass> array_;
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

	class vpd_bone_post_impl : public vpd_bone_post
	{
		std::u8string bone_name_{};
		Vector32 position_{};
		Vector4d32 rotation_{};
	public:
		/**
		 * \brief Constructor function
		 */
		vpd_bone_post_impl() = default;
		/**
		 * \brief Destructor function
		 */
		~vpd_bone_post_impl() override = default;
		/**
		 * \brief Copy constructor
		 */
		vpd_bone_post_impl(const vpd_bone_post_impl&) noexcept = default;
		/**
		 * \brief Move constructor
		 */
		vpd_bone_post_impl(vpd_bone_post_impl&&) noexcept = default;
		/**
		* \brief Copy operator=
		* \return Result reference
		*/
		vpd_bone_post_impl& operator =(const vpd_bone_post_impl&) = default;
		/**
		 * \brief Move operator=
		 * \return Result reference
		 */
		vpd_bone_post_impl& operator =(vpd_bone_post_impl&&) noexcept = default;

		std::string get_bone_name() override;
		void set_bone_name(const std::string& name) override;

		const std::array<float, 3>& get_position() const override;
		void set_position(const std::array<float, 3>& position) override;

		const std::array<float, 4>& get_rotation() const override;
		void set_rotation(const std::array<float, 4>& rotation) override;

		friend class vpd_post_impl;

	};

	class vpd_morph_post_impl : public vpd_morph_post
	{
		std::u8string morph_name_{};
		Float32 morph_weight_{};
	public:
		/**
		 * \brief Constructor function
		 */
		vpd_morph_post_impl() = default;
		/**
		 * \brief Destructor function
		 */
		~vpd_morph_post_impl() override = default;
		/**
		 * \brief Copy constructor
		 */
		vpd_morph_post_impl(const vpd_morph_post_impl&) noexcept = default;
		/**
		 * \brief Move constructor
		 */
		vpd_morph_post_impl(vpd_morph_post_impl&&) noexcept = default;
		/**
		* \brief Copy operator=
		* \return Result reference
		*/
		vpd_morph_post_impl& operator =(const vpd_morph_post_impl&) = default;
		/**
		 * \brief Move operator=
		 * \return Result reference
		 */
		vpd_morph_post_impl& operator =(vpd_morph_post_impl&&) noexcept = default;

		std::string get_morph_name() override;
		void set_morph_name(const std::string& name) override;

		float get_weight() const override;
		void set_weight(float weight) override;

		friend class vpd_post_impl;
	};

	class vpd_post_impl final : public vmd_post
	{
		std::u8string model_name_{};
		vpd_element_array_impl<vpd_bone_post, vpd_bone_post_impl> bones_;
		vpd_element_array_impl<vpd_morph_post, vpd_morph_post_impl> morphs_;
	public:
		/**
		 * \brief Constructor function
		 */
		vpd_post_impl() = default;
		/**
		 * \brief Destructor function
		 */
		~vpd_post_impl() override = default;
		/**
		 * \brief Copy constructor
		 */
		vpd_post_impl(const vpd_post_impl&) noexcept = default;
		/**
		 * \brief Move constructor
		 */
		vpd_post_impl(vpd_post_impl&&) noexcept = default;
		/**
		* \brief Copy operator=
		* \return Result reference
		*/
		vpd_post_impl& operator =(const vpd_post_impl&) = default;
		/**
		 * \brief Move operator=
		 * \return Result reference
		 */
		vpd_post_impl& operator =(vpd_post_impl&&) noexcept = default;

		std::string get_model_name() const override;
		void set_model_name(const std::string& name) override;

		const vpd_bone_post_array& get_vpd_bone_post_array() override;
		vpd_bone_post_array& mutable_vpd_bone_post_array() override;

		const vpd_morph_post_array& get_vpd_morph_post_array() override;
		vpd_morph_post_array& mutable_vpd_morph_post_array() override;

		bool read_from_file(const std::string& file_name) override;
		bool write_to_file(const std::string& file_name) const override;

		bool read_from_file(const std::wstring& file_name) override;
		bool write_to_file(const std::wstring& file_name) const override;

	private:
		bool read_from_file_impl(const path& path);
		bool write_to_file_impl(const path& path) const;
	};
}
