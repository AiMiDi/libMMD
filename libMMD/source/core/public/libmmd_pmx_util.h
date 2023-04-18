/**************************************************************************

Copyright:Copyright(c) 2023-present, Aimidi & libmmd contributors.
Author:			Aimidi
Date:			2023/4/18
File:			libmmd_pmx_util.h
Description:	PMX util

**************************************************************************/

#pragma once

namespace libmmd
{
	class pmx_text_reader
	{
		bool text_encoding_;
	public:
		explicit pmx_text_reader(const bool& text_encoding = false) :text_encoding_(text_encoding)
		{
		}
		void set_text_encoding(const bool& text_encoding = false)
		{
			text_encoding_ = text_encoding;
		}
		bool operator()(const file& file) const;

		bool operator()(const file& file, std::u8string& out_string) const;
	};

	class pmx_index_reader
	{
		Char index_size_;
	public:
		explicit pmx_index_reader(const Char& index_size = 4) :index_size_(index_size)
		{
		}
		void set_index_size(const Char& index_size = 4)
		{
			index_size_ = index_size;
		}
		bool operator()(const file& file) const;

		bool operator()(const file& file, Int32& out_index) const;
	};

	class pmx_unsigned_index_reader
	{
		Char index_size_;
	public:
		explicit pmx_unsigned_index_reader(const Char& index_size = 4) : index_size_(index_size)
		{
		}
		void set_index_size(const Char& index_size = 4)
		{
			index_size_ = index_size;
		}
		bool operator()(const file& file) const;

		bool operator()(const file& file, UInt32& out_index) const;
	};
}
