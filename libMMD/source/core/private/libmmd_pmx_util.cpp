﻿/**************************************************************************

Copyright:Copyright(c) 2023-present, Aimidi & libmmd contributors.
Author:			Aimidi
Date:			2023/4/18
File:			libmmd_pmx_util.cpp
Description:	PMX util

**************************************************************************/

#include "libmmd_pmx_util.h"

namespace libmmd
{
	bool pmx_text_reader::operator()(const file& file) const
	{
		// text字符串最大长度
		Int32 text_length = 0;
		file.read_elements(text_length);
		return file.seek(text_length);
	}

	bool pmx_text_reader::operator()(const file& file, std::u8string& out_string) const
	{
		// text字符串最大长度
		Int32 text_length = 0;
		file.read_elements(text_length);
		if (text_length > 0) {
			if (text_encoding_ == false)
			{
				std::basic_string utf16_string(text_length, u'\0');
				file.read_elements(utf16_string.data(), text_length);
				out_string = code_converter::utf16_le_to_utf8(utf16_string);
				return true;
			}
			out_string.resize(text_length, '\0');
			file.read_elements(out_string.data(), text_length);
			return true;
		}
		out_string.clear();
		return true;
	}

	bool pmx_index_reader::operator()(const file& file) const
	{
		return file.seek(index_size_);
	}

	bool pmx_index_reader::operator()(const file& file, Int32& out_index) const
	{
		// 3种长度不同的Index
		switch (index_size_)
		{
		case 1:
		{
			Char index;
			file.read_elements(index);
			out_index = index == -1 ? -1 : static_cast<UChar>(index);
			break;
		}
		case 2:
		{
			Int16 index;
			file.read_elements(index);
			out_index = index;
			break;
		}
		case 4:
		{
			Int32 index;
			file.read_elements(index);
			out_index = index;
			break;
		}
		default:
			out_index = -1;
			return false;
		}
		return true;
	}

	bool pmx_unsigned_index_reader::operator()(const file& file) const
	{
		return file.seek(index_size_);
	}

	bool pmx_unsigned_index_reader::operator()(const file& file, UInt32& out_index) const
	{
		// 3种长度不同的Index
		switch (index_size_)
		{
		case 1:
		{
			UChar index;
			file.read_elements(index);
			out_index = index;
			break;
		}
		case 2:
		{
			UInt16 index;
			file.read_elements(index);
			out_index = index;
			break;
		}
		case 4:
		{
			UInt32 index;
			file.read_elements(index);
			out_index = index;
			break;
		}
		default:
			out_index = 0;
			return false;
		}
		return true;
	}
}