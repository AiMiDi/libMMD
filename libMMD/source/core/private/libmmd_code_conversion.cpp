/**************************************************************************

Copyright:Copyright(c) 2022-present, Aimidi & CMT contributors.
Author:			Aimidi
Date:			2022/4/18
File:			libmmd_code_conversion.cpp
Description:	Character coding conversion util

**************************************************************************/

#include "libmmd_code_conversion.h"
#include "unicode/ucnv.h"

namespace libmmd
{
	class ucnv_converter
	{
		UErrorCode status = U_ZERO_ERROR;

		UConverter* ucnv_to;
		UConverter* ucnv_from;

	public:
		ucnv_converter(const ::UChar* to_code, const ::UChar* from_code) :
			ucnv_to(ucnv_openU(to_code, &status)),
			ucnv_from(ucnv_openU(from_code, &status)) {}
		~ucnv_converter()
		{
			ucnv_close(ucnv_to);
			ucnv_close(ucnv_from);
		}
		ucnv_converter(const ucnv_converter&) = delete;
		void operator =(const ucnv_converter&) = delete;
		ucnv_converter(ucnv_converter&&) = delete;
		void operator=(ucnv_converter&&) = delete;

		size_t convert(const char* source, const size_t source_length, char* target, const size_t target_capacity)
		{
			auto source_ptr = source;
			const auto source_end = source + source_length;
			auto target_ptr = target;
			const auto target_end = reinterpret_cast<const char*>(target + target_capacity);
			
			ucnv_convertEx(ucnv_to, ucnv_from, &target_ptr, target_end, &source_ptr, source_end, nullptr, nullptr, nullptr, nullptr, true, true, &status);
			return static_cast<size_t>(target_ptr - target);
		}

		[[nodiscard]] bool is_error() const
		{
			return !U_SUCCESS(status);
		}

		[[nodiscard]] bool is_init_error() const
		{
			return status == U_MEMORY_ALLOCATION_ERROR || status == U_FILE_ACCESS_ERROR;
		}

		[[nodiscard]] bool is_buffer_overflow() const
		{
			return status == U_BUFFER_OVERFLOW_ERROR;
		}
	};

	std::string code_converter::utf8_to_shift_jis(const std::u8string& input_string)
	{
		size_t input_length = input_string.length();
		std::string output_string(input_length, '\0');
		ucnv_converter shift_jis_to_u8_converter(u"SHIFT_JIS", u"UTF8");
		if (shift_jis_to_u8_converter.is_init_error())
		{
			ELOGE << "code_converter::utf8_to_shift_jis(): init converter error!";
			return {};
		}
		std::function<void(ucnv_converter&)> do_convert = 
			[&output_string, &input_string, &input_length, &do_convert](ucnv_converter& converter)
		{
			const size_t output_length = output_string.size();
			char* output_buffer = output_string.data();
			const auto input_buffer = reinterpret_cast<const char*>(input_string.data());
			const size_t convertde_length = converter.convert(input_buffer, input_length, output_buffer, output_length);
			if (converter.is_buffer_overflow())
			{
				// add buffer and retry.
				output_string.resize(output_length * 2, '\0');
				do_convert(converter);
				return;
			}
			output_string.resize(convertde_length, '\0');
		};
		do_convert(shift_jis_to_u8_converter);
		if (shift_jis_to_u8_converter.is_error())
		{
			ELOGE << "code_converter::utf8_to_shift_jis(): convert fail!";
			return {};
		}
		return output_string;
	}

	std::u8string code_converter::shift_jis_to_utf8(const std::string& input_string)
	{
		std::u8string output_string{};
		size_t input_length = input_string.length();
		std::vector buffer_vector(input_length, '\0');
		ucnv_converter u8_to_shift_jis_converter(u"UTF8", u"SHIFT_JIS");
		if (u8_to_shift_jis_converter.is_init_error())
		{
			ELOGE << "code_converter::u8_to_shift_jis_converter(): init converter error!";
			return {};
		}
		std::function<void(ucnv_converter&)> do_convert = 
			[&buffer_vector, &input_string, &input_length, &output_string, &do_convert](ucnv_converter& converter)
		{
			const size_t buffer_length = buffer_vector.size();
			char* output_buffer = buffer_vector.data();
			const char* input_buffer = input_string.data();
			const size_t convertde_length = converter.convert(input_buffer, input_length, output_buffer, buffer_length);
			if (converter.is_buffer_overflow())
			{
				// add buffer and retry.
				buffer_vector.resize(buffer_length * 2, '\0');
				do_convert(converter);
				return;
			}
			output_string.assign(reinterpret_cast<const char8_t*>(output_buffer), convertde_length);
		};
		do_convert(u8_to_shift_jis_converter);
		if (u8_to_shift_jis_converter.is_error())
		{
			ELOGE << "code_converter::shift_jis_to_utf8(): convert fail!";
			return {};
		}
		return output_string;
	}

	std::u16string code_converter::utf8_to_utf16_le(const std::u8string& input_string)
	{
		std::u16string output_string{};
		std::u8string::size_type input_length = input_string.length();

		if (input_length == 0)
		{
			return output_string;
		}

		auto input_string_ptr = reinterpret_cast<const unsigned char*>(input_string.data());
		// Determine whether there is a BOM (if the length is less than 3 bytes).
		if (input_length > 3 && input_string_ptr[0] == 0xEF && input_string_ptr[1] == 0xBB && input_string_ptr[2] == 0xBF) 
		{
			input_string_ptr += 3;
			input_length -= 3;
		}

		// Start conversion
		for (std::u8string::size_type i = 0; i < input_length; ++i) 
		{
			// Fetch the first byte of the UTF8 sequence
			const uint32_t char1 = input_string_ptr[i];	
			if ((char1 & 0x80) == 0) {
				// The highest bit is 0, and only 1 byte represents the UNICODE code point
				output_string.push_back(static_cast<char16_t>(char1));
				continue;
			}
			switch (char1 & 0xF0)
			{
			case 0xF0: // 4-byte characters, 0x10000 to 0x10FFFF
				{
					const uint32_t char2 = input_string_ptr[++i];
					const uint32_t char3 = input_string_ptr[++i];
					const uint32_t char4 = input_string_ptr[++i];
					// Calculate the UNICODE code point value (the first byte is low 3bit, the rest is 6bit)
					if (uint32_t code_point = (char1 & 0x07U) << 18 | (char2 & 0x3FU) << 12 | (char3 & 0x3FU) << 6 | (char4 & 0x3FU); code_point >= 0x10000)
					{
						// In UTF-16, Utility 10000 to U+10FFFF are represented by two 16bit units, surrogate pairs.
						// 1. Subtract 0x10000 from the code point (get a value of length 20bit)
						// 2. The high surrogate is to add the high 10bit in that 20bit to 0xD800 (110110 00000000)
						// 3. The low surrogate is to add the low 10bit in that 20bit to 0xDC00 (110111 00000000)
						code_point -= 0x10000;
						output_string.push_back(static_cast<char16_t>(code_point >> 10 | 0xD800U));
						output_string.push_back(static_cast<char16_t>((code_point & 0x03FFU) | 0xDC00U));
					}
					else
					{
						// In UTF-16, the values of Ubun0000 to U+D7FF and U+E000 to U+FFFF are the same as Unicode code points.
						// U+D800 to U+DFFF is an invalid character, and for simplicity, it is assumed that it does not exist (if so, it is not encoded)
						output_string.push_back(static_cast<char16_t>(code_point));
					}
				}
				break;
			case 0xE0: // 3-byte characters, 0x800 to 0xFFFF
				{
					const uint32_t char2 = input_string_ptr[++i];
					const uint32_t char3 = input_string_ptr[++i];
					// Calculate the UNICODE code point value (the first byte is low 4bit, the rest is 6bit)
					const uint32_t code_point = (char1 & 0x0FU) << 12 | (char2 & 0x3FU) << 6 | (char3 & 0x3FU);
					output_string.push_back(static_cast<char16_t>(code_point));
				}
				break;
			case 0xD0: // 2-byte characters, 0x80 to 0x7FF
			case 0xC0:
				{
					const uint32_t char2 = input_string_ptr[++i];
					// Calculate the UNICODE code point value (the first byte is low 5bit, the rest is 6bit)
					const uint32_t code_point = (char1 & 0x1FU) << 12 | (char2 & 0x3FU) << 6;
					output_string.push_back(static_cast<char16_t>(code_point));
				}
				break;
			default:	// Single-byte part (it has been dealt with before, so it should not come in)
				break;
			}
		}

		return output_string;
	}

	std::u8string code_converter::utf16_le_to_utf8(const std::u16string& input_string)
	{
		std::u8string output_string{};
		std::u16string::size_type input_length = input_string.length();

		if (input_length == 0)
		{
			return output_string;
		}

		const char16_t* input_string_ptr = input_string.data();
		if (input_string_ptr[0] == 0xFEFF) 
		{
			input_string_ptr += 1;	// With bom tag, move back
			input_length -= 1;
		}

		// Start conversion
		output_string.reserve(input_length * 3);
		for (std::u16string::size_type i = 0; i < input_length; ++i) 
		{
			// It is assumed here that it is in the small end order (the large end order is not applicable)

			// 1 byte representation part
			if (const char16_t u16_char = input_string_ptr[i]; u16_char < 0x0080) 
			{
				// u16char <= 0x007f
				// U- 0000 0000 ~ 0000 07ff : 0xxx xxxx
				output_string.push_back(static_cast<char8_t>(u16_char & 0x00FF));	// Lower 8bit
			}
			// 2-word energy saving representation part
			else if (u16_char >= 0x0080 && u16_char <= 0x07FF) {
				// * U-00000080 - U-000007FF:  110xxxxx 10xxxxxx
				output_string.push_back(static_cast<char8_t>(((u16_char >> 6) & 0x1F) | 0xC0));
				output_string.push_back(static_cast<char8_t>((u16_char & 0x3F) | 0x80));
			}
			// Surrogate pair part (4 bytes)
			else if (u16_char >= 0xD800 && u16_char <= 0xDBFF) {
				// * U-00010000 - U-001FFFFF: 1111 0xxx 10xxxxxx 10xxxxxx 10xxxxxx
				const uint32_t high_sur = u16_char;
				const uint32_t low_sur = input_string_ptr[++i];
				// Conversion from surrogate pairs to UNICODE code points
				// 1. Subtract 0xD800 from the high surrogate to get a valid 10bit
				// 2. Subtract the 0xDC00 from the low surrogate to get the valid 10bit
				// 3. Add 0x10000 to get the UNICODE code point value
				uint32_t code_point = high_sur - 0xD800;
				code_point <<= 10;
				code_point |= low_sur - 0xDC00;
				code_point += 0x10000;
				// 转为4字节UTF8编码表示
				output_string.push_back(static_cast<char8_t>(code_point >> 18 | 0xF0));
				output_string.push_back(static_cast<char8_t>(((code_point >> 12) & 0x3F) | 0x80));
				output_string.push_back(static_cast<char8_t>(((code_point >> 06) & 0x3F) | 0x80));
				output_string.push_back(static_cast<char8_t>((code_point & 0x3F) | 0x80));
				continue;
			}
			// 3-byte representation part
			else
			{
				// * U-0000E000 - U-0000FFFF:  1110xxxx 10xxxxxx 10xxxxxx
				output_string.push_back(static_cast<char8_t>(((u16_char >> 12) & 0x0F) | 0xE0));
				output_string.push_back(static_cast<char8_t>(((u16_char >> 6) & 0x3F) | 0x80));
				output_string.push_back(static_cast<char8_t>((u16_char & 0x3F) | 0x80));
			}
		}
		return output_string;
	}
}
