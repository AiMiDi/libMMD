/**************************************************************************

Copyright:Copyright(c) 2023-present, Aimidi & libMMD contributors.
Author:			Aimidi
Date:			2023/7/28
File:			libmmd_conversion.h
Description:	code conversion interface

**************************************************************************/

#pragma once

#include "libmmd_marco.h"
#include <string>

namespace libmmd
{
	/**
	* \brief Converts a UTF8 string to an SHIFT-JIS string.
	* \param input_string Source UTF8 string
	* \return Result SHIFT-JIS string
	*/
	LIBMMD_API std::string utf8_to_shift_jis(const std::string& input_string);
	/**
	 * \brief Converts a SHIFT-JIS string to an UTF8 string.
	 * \param input_string Source SHIFT-JIS string
	 * \return Result UTF8 string
	 */
	LIBMMD_API std::string shift_jis_to_utf8(const std::string& input_string);
	/**
	 * \brief Converts a UTF8 string to an UTF16LE string.
	 * \param input_string Source UTF8 string
	 * \return Result UTF16LE string
	 */
	LIBMMD_API std::u16string utf8_to_utf16_le(const std::string& input_string);
	/**
	 * \brief Converts a UTF16LE string to an UTF8 string.
	 * \param input_string Source UTF16LE string
	 * \return Result UTF8 string
	 */
	LIBMMD_API std::string utf16_le_to_utf8(const std::u16string& input_string);
}