/**************************************************************************

Copyright:Copyright(c) 2023-present, Aimidi & libMMD contributors.
Author:			Aimidi
Date:			2023/9/04
File:			libmmd_assets_file.h
Description:	mmd assets file interface

**************************************************************************/

#pragma once

namespace libmmd
{
	class LIBMMD_API mmd_assets_file
	{
	public:
		virtual ~mmd_assets_file() = default;

		virtual bool read_from_file_u8(const std::string& file_name) = 0;
		LIBMMD_NODISCARD virtual bool write_to_file_u8(const std::string& file_name) const = 0;

		virtual bool read_from_file(const std::string& file_name) = 0;
		LIBMMD_NODISCARD virtual bool write_to_file(const std::string& file_name) const = 0;

		virtual bool read_from_file(const std::wstring& file_name) = 0;
		LIBMMD_NODISCARD virtual bool write_to_file(const std::wstring& file_name) const = 0;

#ifdef __cpp_lib_string_view
		virtual bool read_from_file_u8(const std::string_view& file_name) = 0;
		LIBMMD_NODISCARD virtual bool write_to_file_u8(const std::string_view& file_name) const = 0;

		virtual bool read_from_file(const std::string_view& file_name) = 0;
		LIBMMD_NODISCARD virtual bool write_to_file(const std::string_view& file_name) const = 0;

		virtual bool read_from_file(const std::wstring_view& file_name) = 0;
		LIBMMD_NODISCARD virtual bool write_to_file(const std::wstring_view& file_name) const = 0;
#endif
	};
}
