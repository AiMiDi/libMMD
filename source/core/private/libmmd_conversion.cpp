#include "libMMD/libmmd_conversion.h"

namespace libmmd
{
	std::string utf8_to_shift_jis(const std::string& input_string)
	{
		return code_converter::utf8_to_shift_jis({ input_string.begin(),input_string.end() });
	}

	std::string shift_jis_to_utf8(const std::string& input_string)
	{
		auto results = code_converter::shift_jis_to_utf8(input_string);
		return { results.begin(),results.end() };
	}

	std::u16string utf8_to_utf16_le(const std::string& input_string)
	{
		return code_converter::utf8_to_utf16_le({ input_string.begin(),input_string.end() });
	}

	std::string utf16_le_to_utf8(const std::u16string& input_string)
	{
		auto results = code_converter::utf16_le_to_utf8(input_string);
		return { results.begin(),results.end() };
	}
}
