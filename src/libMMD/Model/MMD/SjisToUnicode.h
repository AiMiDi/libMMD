//
// Copyright(c) 2016-2017 benikabocha.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)
//

#ifndef LIBMMD_MODEL_MMD_SJISTOUNICODE_H_
#define LIBMMD_MODEL_MMD_SJISTOUNICODE_H_

#include <string>

namespace libmmd
{
	/**
	 * @brief Convert a single Shift-JIS character to a UTF-16 character.
	 * @param ch Shift-JIS character.
	 * @return UTF-16 character.
	 */
	char16_t ConvertSjisToU16Char(int ch);

	/**
	 * @brief Convert a Shift-JIS string to a UTF-16 string.
	 * @param sjisCode Pointer to the Shift-JIS string.
	 * @return UTF-16 string.
	 */
	std::u16string ConvertSjisToU16String(const char* sjisCode);

	/**
	 * @brief Convert a Shift-JIS string to a UTF-32 string.
	 * @param sjisCode Pointer to the Shift-JIS string.
	 * @return UTF-32 string.
	 */
	std::u32string ConvertSjisToU32String(const char* sjisCode);

	/**
	 * @brief Convert a UTF-16 string to a Shift-JIS string.
	 * @param u16Str The UTF-16 string to convert.
	 * @return Shift-JIS encoded string. Characters without a mapping are replaced with '?'.
	 */
	std::string ConvertU16ToSjisString(const std::u16string& u16Str);
}

#endif // !LIBMMD_MODEL_MMD_SJISTOUNICODE_H_
