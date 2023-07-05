/**************************************************************************

Copyright:Copyright(c) 2022-present, Aimidi & Walter White & CMT contributors.
Author:			Aimidi
Date:			2022/7/15
File:			vmd_morph_animation.h
Description:	MMD style expression animation

**************************************************************************/

#pragma once

#include "vmd_element.hpp"

namespace libmmd
{
	class vmd_morph_key_frame_impl : public vmd_morph_key_frame, public vmd_key_frame_impl
	{
		// The action corresponds to the expression name
		std::u8string	morph_name_{};
		// Expression deformation strength
		Float32 weight_{};
	public:
		std::string get_morph_name() const override;
		void set_morph_name(const std::string& name) override;

		float get_weight() const override;
		void set_weight(float weight) override;

		bool read_from_file(const file& file) override;
		[[nodiscard]] bool write_to_file(const file& file) const override;
	};
}
