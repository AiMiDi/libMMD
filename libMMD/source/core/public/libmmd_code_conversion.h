/**************************************************************************

Copyright:Copyright(c) 2022-present, Aimidi & Walter White & CMT contributors.
Author:			Aimidi
Date:			2022/7/2
File:			libmmd_code_conversion.h
Description:	Character coding conversion util

**************************************************************************/

#pragma once

namespace libmmd
{
	class code_converter
	{
	public:
		/**
		 * \brief Converts a UTF8 string to an SHIFT-JIS string.
		 * \param input_string Source UTF8 string
		 * \return Result SHIFT-JIS string
		 */
		static std::string utf8_to_shift_jis(const std::u8string& input_string);
		/**
		 * \brief Converts a SHIFT-JIS string to an UTF8 string.
		 * \param input_string Source SHIFT-JIS string
		 * \return Result UTF8 string
		 */
		static std::u8string shift_jis_to_utf8(const std::string& input_string);
		/**
		 * \brief Converts a UTF8 string to an UTF16LE string.
		 * \param input_string Source UTF8 string
		 * \return Result UTF16LE string
		 */
		static std::u16string utf8_to_utf16_le(const std::u8string& input_string);
		/**
		 * \brief Converts a UTF16LE string to an UTF8 string.
		 * \param input_string Source UTF16LE string
		 * \return Result UTF8 string
		 */
		static std::u8string utf16_le_to_utf8(const std::u16string& input_string);

		code_converter(const code_converter&) = delete;
		void operator =(const code_converter&) = delete;
		code_converter(code_converter&&) = delete;
		void operator=(code_converter&&) = delete;
	private:
		code_converter() = default;
		~code_converter() = default;

		class iconv_converter
		{
			libiconv_t conv_;
		public:
			iconv_converter(const char* to_code, const char* from_code) :
				conv_(libiconv_open(to_code, from_code)) {}
			~iconv_converter()
			{
				libiconv_close(conv_);
			}
			size_t convert(const char** in_buf, size_t* in_bytes_left, char** out_buf, size_t* out_bytes_left) const
			{
				return libiconv(conv_, in_buf, in_bytes_left, out_buf, out_bytes_left);
			}

		};
	};
}
