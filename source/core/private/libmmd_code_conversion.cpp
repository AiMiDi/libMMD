/**************************************************************************

Copyright:Copyright(c) 2022-present, Aimidi & libMMD contributors.
Author:			Aimidi
Date:			2023/7/27
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
		if (input_length == 0)
		{
			return {};
		}
		std::string output_string(input_length, '\0');
		ucnv_converter shift_jis_to_u8_converter(u"Shift_JIS", u"UTF8");
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
		if (input_length == 0)
		{
			return output_string;
		}

		std::vector buffer_vector(input_length, '\0');
		ucnv_converter u8_to_shift_jis_converter(u"UTF8", u"Shift_JIS");
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
		size_t input_length = input_string.length();
		if (input_length == 0)
		{
			return output_string;
		}

		std::vector buffer_vector(input_length, '\0');
		ucnv_converter u8_to_u16le_converter(u"UTF16LE", u"UTF8");
		if (u8_to_u16le_converter.is_init_error())
		{
			ELOGE << "code_converter::utf8_to_utf16_le(): init converter error!";
			return {};
		}
		std::function<void(ucnv_converter&)> do_convert =
			[&buffer_vector, &input_string, &input_length, &output_string, &do_convert](ucnv_converter& converter)
		{
			const size_t buffer_length = buffer_vector.size();
			char* output_buffer = buffer_vector.data();
			const auto input_buffer = reinterpret_cast<const char*>(input_string.data());
			const size_t convertde_length = converter.convert(input_buffer, input_length, output_buffer, buffer_length);
			if (converter.is_buffer_overflow())
			{
				// add buffer and retry.
				buffer_vector.resize(buffer_length * 2, '\0');
				do_convert(converter);
				return;
			}
			output_string.assign(reinterpret_cast<const char16_t*>(output_buffer), convertde_length);
		};
		do_convert(u8_to_u16le_converter);
		if (u8_to_u16le_converter.is_error())
		{
			ELOGE << "code_converter::utf8_to_utf16_le(): convert fail!";
			return {};
		}
		return output_string;
	}

	std::u8string code_converter::utf16_le_to_utf8(const std::u16string& input_string)
	{
		std::u8string output_string{};
		size_t input_length = input_string.length();
		if (input_length == 0)
		{
			return output_string;
		}

		std::vector buffer_vector(input_length, '\0');
		ucnv_converter u16le_to_u8_converter(u"UTF8", u"UTF16LE");
		if (u16le_to_u8_converter.is_init_error())
		{
			ELOGE << "code_converter::utf16_le_to_utf8(): init converter error!";
			return {};
		}
		std::function<void(ucnv_converter&)> do_convert =
			[&buffer_vector, &input_string, &input_length, &output_string, &do_convert](ucnv_converter& converter)
		{
			const size_t buffer_length = buffer_vector.size();
			char* output_buffer = buffer_vector.data();
			const auto input_buffer = reinterpret_cast<const char*>(input_string.data());
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
		do_convert(u16le_to_u8_converter);
		if (u16le_to_u8_converter.is_error())
		{
			ELOGE << "code_converter::utf16_le_to_utf8(): convert fail!";
			return {};
		}
		return output_string;
	}
}
