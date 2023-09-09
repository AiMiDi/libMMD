/**************************************************************************

Copyright:Copyright(c) 2023-present, Aimidi & libMMD contributors.
Author:			Aimidi
Date:			2023/9/04
File:			mmd_assets_file.h
Description:	mmd assets file impl

**************************************************************************/

#pragma once

namespace libmmd
{
	class mmd_assets_file_impl : virtual public mmd_assets_file
	{
	public:
		/**
		 * \brief Constructor function
		 */
		mmd_assets_file_impl() = default;
		/**
		 * \brief Destructor function
		 */
		~mmd_assets_file_impl() override = default;
		/**
		 * \brief Copy constructor
		 */
		mmd_assets_file_impl(const mmd_assets_file_impl&) noexcept = default;
		/**
		 * \brief Move constructor
		 */
		mmd_assets_file_impl(mmd_assets_file_impl&&) noexcept = default;
		/**
		* \brief Copy operator=
		* \return Result reference
		*/
		mmd_assets_file_impl& operator =(const mmd_assets_file_impl&) = default;
		/**
		 * \brief Move operator=
		 * \return Result reference
		 */
		mmd_assets_file_impl& operator =(mmd_assets_file_impl&&) noexcept = default;

		bool read_from_file_u8(const std::string& file_name) override;
		[[nodiscard]] bool write_to_file_u8(const std::string& file_name) const override;

		bool read_from_file(const std::string& file_name) override;
		[[nodiscard]] bool write_to_file(const std::string& file_name) const override;

		bool read_from_file(const std::wstring& file_name) override;
		[[nodiscard]] bool write_to_file(const std::wstring& file_name) const override;

#ifdef __cpp_lib_string_view
		bool read_from_file_u8(const std::string_view& file_name) override;
		[[nodiscard]] bool write_to_file_u8(const std::string_view& file_name) const override;

		bool read_from_file(const std::string_view& file_name) override;
		[[nodiscard]] bool write_to_file(const std::string_view& file_name) const override;

		bool read_from_file(const std::wstring_view& file_name) override;
		[[nodiscard]] bool write_to_file(const std::wstring_view& file_name) const override;
#endif

	private:
		virtual bool read_from_file_impl(const path& path) = 0;
		[[nodiscard]] virtual bool write_to_file_impl(const path& path) const = 0;
	};
	
}
