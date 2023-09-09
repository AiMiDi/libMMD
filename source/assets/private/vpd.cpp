/**************************************************************************

Copyright:Copyright(c) 2022-present, Aimidi & Walter White & CMT contributors.
Author:			Aimidi
Date:			2022/7/2
File:			vpd_data.cpp
Description:	vpd file data

**************************************************************************/

#include "vpd.h"

namespace libmmd
{
	std::string vpd_bone_post_impl::get_bone_name()
	{
		return std::string{bone_name_.begin(), bone_name_.end()};
	}

	void vpd_bone_post_impl::set_bone_name(const std::string& name)
	{
		bone_name_ = std::u8string{ name.begin(), name.end() };
	}

	const std::array<float, 3>& vpd_bone_post_impl::get_position() const
	{
		return position_;
	}

	void vpd_bone_post_impl::set_position(const std::array<float, 3>& position)
	{
		position_ = position;
	}

	const std::array<float, 4>& vpd_bone_post_impl::get_rotation() const
	{
		return rotation_;
	}

	void vpd_bone_post_impl::set_rotation(const std::array<float, 4>& rotation)
	{
		rotation_ = rotation;
	}

	std::string vpd_morph_post_impl::get_morph_name()
	{
		return std::string{morph_name_.begin(), morph_name_.end()};
	}

	void vpd_morph_post_impl::set_morph_name(const std::string& name)
	{
		morph_name_ = std::u8string{ name.begin(), name.end() };
	}

	float vpd_morph_post_impl::get_weight() const
	{
		return morph_weight_;
	}

	void vpd_morph_post_impl::set_weight(float weight)
	{
		morph_weight_ = weight;
	}

	bool vpd_post_impl::read_from_file_impl(const path& path) {
		file file;
		if (!path.check_suffix(".vpd"))
			return false;

		if (!file.open(path, file::open_mode::READ))
			return false;

		const auto file_length = file.get_length();
		/* 申请文件长度的内存 */
		std::string file_jis_string(file_length, '\0');
		/* 将整个文件读取到申请的内存 */
		file.read_elements(std::span(file_jis_string.begin(), file_jis_string.end()), file_length);
		/* 将读取的字符转为UTF8 */
		const std::u8string file_u8_string = code_converter::shift_jis_to_utf8(file_jis_string);
		/* file_length用于判断是否到达文件尾 */
		size_t is_read = 0;
		const size_t file_length_size = file_u8_string.length();
		std::vector<std::u8string_view> lines;
		while (is_read < file_length_size)
		{
			std::u8string_view line;
			/* 遇到 /r /n 截断字符串为一行并跳过 \r \n。 */
			if (size_t read_pos = file_u8_string.find_first_of('\r', is_read); read_pos)
			{
				line = std::u8string_view(file_u8_string.data() + is_read, read_pos - is_read);
				is_read = read_pos + 2;
			}
			/* 遇到 /n 截断字符串为一行并跳过 \n。 */
			else if (read_pos = file_u8_string.find_first_of('\n', is_read); read_pos)
			{
				line = std::u8string_view(file_u8_string.data() + is_read, read_pos - is_read);
				is_read = read_pos + 1;

			}
			/* 如果非空则插入结果 */
			if (!line.empty())
			{
				lines.push_back(line);
			}
		}
		if (lines.empty() || lines[0].compare(0, 24, u8"Vocaloid Pose Data file") != 0)
		{
			ELOGD << "VPD::load_from_file(): error at VPD file format error.";
			return false;
		}
		/* remove comment */
		for (auto& line : lines) {
			if (const size_t comment_pos = line.find_first_of(u8"//"); comment_pos) {
				line = line.substr(0, comment_pos);
			}
		}
		/* get model name */
		size_t semicolon_pos = lines[1].find_first_of(';');
		model_name_ = lines[1].substr(0, semicolon_pos).data();

		const auto line_count = lines.size();
		for (size_t i = 3; i < line_count; i += 4)
		{
			auto& [name, translate, rotation] = bones_.add_impl();
			const size_t curly_braces_pos = lines[i].find_first_of('{');
			name = lines[i].substr(curly_braces_pos + 1, lines[i].length() - curly_braces_pos).data();
			size_t comma_pos = lines[i + 1].find_first_of(',');
			translate[0] = std::stof(reinterpret_cast<const char*>(lines[i + 1].substr(1, comma_pos - 1).data()));
			size_t prec_comma_pos = comma_pos + 1;
			comma_pos = lines[i + 1].find_first_of(',', prec_comma_pos);
			translate[1] = std::stof(reinterpret_cast<const char*>(lines[i + 1].substr(prec_comma_pos, comma_pos - prec_comma_pos).data()));
			prec_comma_pos = comma_pos + 1;
			semicolon_pos = lines[i + 1].find_first_of(';', prec_comma_pos);
			translate[2] = std::stof(reinterpret_cast<const char*>(lines[i + 1].substr(prec_comma_pos, semicolon_pos - prec_comma_pos).data()));
			comma_pos = lines[i + 2].find_first_of(',');
			rotation[0] = std::stof(reinterpret_cast<const char*>(lines[i + 2].substr(1, comma_pos - 1).data()));
			prec_comma_pos = comma_pos + 1;
			comma_pos = lines[i + 2].find_first_of(',', prec_comma_pos);
			rotation[1] = std::stof(reinterpret_cast<const char*>(lines[i + 2].substr(prec_comma_pos, comma_pos - prec_comma_pos).data()));
			prec_comma_pos = comma_pos + 1;
			comma_pos = lines[i + 2].find_first_of(',', prec_comma_pos);
			rotation[2] = std::stof(reinterpret_cast<const char*>(lines[i + 2].substr(prec_comma_pos, comma_pos - prec_comma_pos).data()));
			prec_comma_pos = comma_pos + 1;
			semicolon_pos = lines[i + 2].find_first_of(';', prec_comma_pos);
			rotation[3] = std::stof(reinterpret_cast<const char*>(lines[i + 2].substr(prec_comma_pos, semicolon_pos - prec_comma_pos).data()));
		}
		return true;
	}
	bool vpd_post_impl::write_to_file_impl(const path& path) const
	{
		std::u8string file_string { u8"Vocaloid Pose Data file\r\n" };
		file file;
		if (!path.check_suffix(".vpd"))
			return false;

		if (!file.open(path, file::open_mode::WRITE))
			return false;

		file.write_elements(std::span(file_string.begin(), file_string.end()), file_string.length());
		return true;
	}

	std::string vpd_post_impl::get_model_name() const
	{
		return std::string{model_name_.begin(), model_name_.end()};
	}

	void vpd_post_impl::set_model_name(const std::string& name)
	{
		model_name_ = std::u8string{ name.begin(), name.end() };
	}

	const vpd_post::vpd_bone_post_array& vpd_post_impl::get_vpd_bone_post_array() const
	{
		return bones_;
	}

	vpd_post::vpd_bone_post_array& vpd_post_impl::mutable_vpd_bone_post_array()
	{
		return bones_;
	}

	const vpd_post::vpd_morph_post_array& vpd_post_impl::get_vpd_morph_post_array() const
	{
		return morphs_;
	}

	vpd_post::vpd_morph_post_array& vpd_post_impl::mutable_vpd_morph_post_array()
	{
		return morphs_;
	}
}
