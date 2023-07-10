/**************************************************************************

Copyright:Copyright(c) 2022-present, Aimidi & Walter White & CMT contributors.
Author:			Aimidi
Date:			2022/7/2
File:			vpd_data.h
Description:	vpd file data

**************************************************************************/

#pragma once
#include "vpd_element.hpp"

namespace libmmd
{
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
