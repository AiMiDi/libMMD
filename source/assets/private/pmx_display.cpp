/**************************************************************************

Copyright:Copyright(c) 2022-present, Aimidi & Walter White & CMT contributors.
Author:			Aimidi
Date:			2022/9/19
File:			pmx_display.cpp
Description:	pmx display data

**************************************************************************/

#include "pmx_display.h"

namespace libmmd
{
	 pmx_display_item::display_frame_type pmx_display_item_impl::get_frame_type() const
	{
		return frame_type_;
	}

	 void pmx_display_item_impl::set_frame_type(const display_frame_type type)
	{
		frame_type_ = type;
	}

	 int32_t pmx_display_item_impl::get_index() const
	{
		return index_;
	}

	 void pmx_display_item_impl::set_index(int32_t data)
	{
		index_ = data;
	}

	bool pmx_display_item_impl::read_from_file(const file& file)
	{
		if (!file.read_elements(frame_type_))
			return false;
		switch (frame_type_)
		{
		case display_frame_type::BONE:
		{
			model_description_->read_bone_index(file, index_);
			break;
		}
		case display_frame_type::MORPH:
		{
			model_description_->read_morph_index(file, index_);
			break;
		}
		}
		return true;
	}

	bool pmx_display_item_impl::write_to_file(const file& file) const
	{
		// TODO: write to file
		return false;
	}

	 std::string pmx_display_frame_impl::get_display_name_local() const
	{
		return std::string{display_name_local_.begin(), display_name_local_.end()};
	}

	 void pmx_display_frame_impl::set_display_name_local(const std::string& name)
	{
		display_name_local_ = std::u8string{ name.begin(),name.end() };
	}

	 std::string pmx_display_frame_impl::get_display_name_universal() const
	{
		return std::string{display_name_universal_.begin(), display_name_universal_.end()};
	}

	 void pmx_display_frame_impl::set_display_name_universal(const std::string& name)
	{
		display_name_universal_ = std::u8string{ name.begin(),name.end() };
	}

	 pmx_display_frame::special_flag pmx_display_frame_impl::get_special_flag() const
	{
		return special_flag_;
	}

	 void pmx_display_frame_impl::set_special_flag(const special_flag flag)
	{
		special_flag_ = flag;
	}

	 const pmx_display_frame::pmx_display_item_array& pmx_display_frame_impl::get_display_item_array() const
	{
		return frame_item_array_;
	}

	 pmx_display_frame::pmx_display_item_array& pmx_display_frame_impl::mutable_display_item_array()
	{
		return frame_item_array_;
	}

	bool pmx_display_frame_impl::read_from_file(const file& file)
	{
		if (!model_description_->read_text(file, display_name_local_))
			return false;
		if (!model_description_->read_text(file, display_name_universal_))
			return false;
		if (!file.read_elements(special_flag_))
			return false;
		auto frame_count = Int32();
		if (!file.read_elements(frame_count))
			return false;

		for (Int32 frame_index = 0; frame_index < frame_count; frame_index++)
		{
			if(!frame_item_array_.add_impl().read_from_file(file))
				return false;
		}
		return true;
	}

	auto pmx_display_frame_impl::write_to_file(const file& file) const -> bool
	{
		// TODO: write to file
		return false;
	}
}
