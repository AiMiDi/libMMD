/**************************************************************************

Copyright:Copyright(c) 2022-present, Aimidi & CMT contributors.
Author:			Aimidi
Date:			2022/4/18
File:			libmmd_code_conversion.cpp
Description:	Character coding conversion util

**************************************************************************/

#include "libmmd_code_conversion.h"
#include "libiconv/iconv.h"

namespace libmmd
{
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
		iconv_converter(const iconv_converter&) = delete;
		void operator =(const iconv_converter&) = delete;
		iconv_converter(iconv_converter&&) = delete;
		void operator=(iconv_converter&&) = delete;

		size_t convert(const char** in_buf, size_t* in_bytes_left, char** out_buf, size_t* out_bytes_left) const
		{
			return libiconv(conv_, in_buf, in_bytes_left, out_buf, out_bytes_left);
		}
	};

	std::string code_converter::utf8_to_shift_jis(const std::u8string& input_string)
	{
		size_t input_length = input_string.length();
		std::string output_string(input_length, '\0');
		const iconv_converter shift_jis_to_u8_converter("SHIFT_JIS", "UTF-8");
		std::function<void(const iconv_converter&)> do_convert = 
			[&output_string, &input_string, &input_length, &do_convert](const iconv_converter& converter)
		{
			size_t output_length = output_string.size();
			char* output_buffer = output_string.data();
			auto input_buffer = reinterpret_cast<const char*>(input_string.data());
			if (converter.convert(&input_buffer, &input_length, &output_buffer, &output_length) == static_cast<size_t>(-1))
			{
				if (errno == E2BIG)
				{
					output_string.resize(output_length * 2, '\0');
					// retry
					do_convert(converter);
				}
				return;
			}
			output_string.resize(output_length, '\0');
		};
		do_convert(shift_jis_to_u8_converter);
		return output_string;
	}

	std::u8string code_converter::shift_jis_to_utf8(const std::string& input_string)
	{
		std::u8string output_string{};
		size_t input_length = input_string.length();
		std::vector buffer_vector(input_length, '\0');
		const iconv_converter u8_to_shift_jis_converter("UTF-8", "SHIFT_JIS");
		std::function<void(const iconv_converter&)> do_convert = 
			[&buffer_vector, &input_string, &input_length, &output_string, &do_convert](const iconv_converter& converter)
		{
			size_t buffer_length = buffer_vector.size();
			char* output_men = buffer_vector.data();
			char* output_buffer = output_men;
			const char* input_buffer = input_string.data();
			if (converter.convert(&input_buffer, &input_length, &output_buffer, &buffer_length) == static_cast<size_t>(-1))
			{
				if (errno == E2BIG)
				{
					buffer_vector.resize(buffer_length * 2, '\0');
					// retry
					do_convert(converter);
				}
				return;
			}
			const size_t convected_size = buffer_vector.size() - buffer_length;
			output_string.assign(reinterpret_cast<const char8_t*>(output_men), convected_size);
		};
		do_convert(u8_to_shift_jis_converter);
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
		// 判断是否具有BOM(判断长度小于3字节的情况)
		if (input_length > 3 && input_string_ptr[0] == 0xEF && input_string_ptr[1] == 0xBB && input_string_ptr[2] == 0xBF) 
		{
			input_string_ptr += 3;
			input_length -= 3;
		}

		// 开始转换
		for (std::u8string::size_type i = 0; i < input_length; ++i) 
		{
			const uint32_t char1 = input_string_ptr[i];	// 取出UTF8序列首字节
			if ((char1 & 0x80) == 0) {
				// 最高位为0，只有1字节表示UNICODE代码点
				output_string.push_back(static_cast<char16_t>(char1));
				continue;
			}
			switch (char1 & 0xF0)
			{
			case 0xF0: // 4 字节字符, 0x10000 到 0x10FFFF
				{
					const uint32_t char2 = input_string_ptr[++i];
					const uint32_t char3 = input_string_ptr[++i];
					const uint32_t char4 = input_string_ptr[++i];
					// 计算UNICODE代码点值(第一个字节取低3bit，其余取6bit)
					if (uint32_t code_point = (char1 & 0x07U) << 18 | (char2 & 0x3FU) << 12 | (char3 & 0x3FU) << 6 | (char4 & 0x3FU); code_point >= 0x10000)
					{
						// 在UTF-16中 U+10000 到 U+10FFFF 用两个16bit单元表示, 代理项对.
						// 1、将代码点减去0x10000(得到长度为20bit的值)
						// 2、high 代理项 是将那20bit中的高10bit加上0xD800(110110 00 00000000)
						// 3、low  代理项 是将那20bit中的低10bit加上0xDC00(110111 00 00000000)
						code_point -= 0x10000;
						output_string.push_back(static_cast<char16_t>(code_point >> 10 | 0xD800U));
						output_string.push_back(static_cast<char16_t>((code_point & 0x03FFU) | 0xDC00U));
					}
					else
					{
						// 在UTF-16中 U+0000 到 U+D7FF 以及 U+E000 到 U+FFFF 与Unicode代码点值相同.
						// U+D800 到 U+DFFF 是无效字符, 为了简单起见，这里假设它不存在(如果有则不编码)
						output_string.push_back(static_cast<char16_t>(code_point));
					}
				}
				break;
			case 0xE0: // 3 字节字符, 0x800 到 0xFFFF
				{
					const uint32_t char2 = input_string_ptr[++i];
					const uint32_t char3 = input_string_ptr[++i];
					// 计算UNICODE代码点值(第一个字节取低4bit，其余取6bit)
					const uint32_t code_point = (char1 & 0x0FU) << 12 | (char2 & 0x3FU) << 6 | (char3 & 0x3FU);
					output_string.push_back(static_cast<char16_t>(code_point));
				}
				break;
			case 0xD0: // 2 字节字符, 0x80 到 0x7FF
			case 0xC0:
				{
					const uint32_t char2 = input_string_ptr[++i];
					// 计算UNICODE代码点值(第一个字节取低5bit，其余取6bit)
					const uint32_t code_point = (char1 & 0x1FU) << 12 | (char2 & 0x3FU) << 6;
					output_string.push_back(static_cast<char16_t>(code_point));
				}
				break;
			default:	// 单字节部分(前面已经处理，所以不应该进来)
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
			input_string_ptr += 1;	//带有bom标记，后移
			input_length -= 1;
		}

		// 开始转换
		output_string.reserve(input_length * 3);
		for (std::u16string::size_type i = 0; i < input_length; ++i) 
		{
			// 这里假设是在小端序下(大端序不适用)

			// 1字节表示部分
			if (const char16_t u16_char = input_string_ptr[i]; u16_char < 0x0080) 
			{
				// u16char <= 0x007f
				// U- 0000 0000 ~ 0000 07ff : 0xxx xxxx
				output_string.push_back(static_cast<char8_t>(u16_char & 0x00FF));	// 取低8bit
			}
			// 2 字节能表示部分
			else if (u16_char >= 0x0080 && u16_char <= 0x07FF) {
				// * U-00000080 - U-000007FF:  110xxxxx 10xxxxxx
				output_string.push_back(static_cast<char8_t>(((u16_char >> 6) & 0x1F) | 0xC0));
				output_string.push_back(static_cast<char8_t>((u16_char & 0x3F) | 0x80));
			}
			// 代理项对部分(4字节表示)
			else if (u16_char >= 0xD800 && u16_char <= 0xDBFF) {
				// * U-00010000 - U-001FFFFF: 1111 0xxx 10xxxxxx 10xxxxxx 10xxxxxx
				const uint32_t high_sur = u16_char;
				const uint32_t low_sur = input_string_ptr[++i];
				// 从代理项对到UNICODE代码点转换
				// 1、从高代理项减去0xD800，获取有效10bit
				// 2、从低代理项减去0xDC00，获取有效10bit
				// 3、加上0x10000，获取UNICODE代码点值
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
			// 3 字节表示部分
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
