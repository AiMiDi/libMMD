/**************************************************************************

Copyright:Copyright(c) 2022-present, Aimidi & Walter White & CMT contributors.
Author:			Aimidi
Date:			2022/7/2
File:			vpd_data.h
Description:	vpd file data

**************************************************************************/

#pragma once

namespace libmmd
{
	/* VPD */
	struct vpd_bone_data
	{
		std::u8string      name{};
		Vector32	translate{};
		Vector4d32	rotation{};
	};
	struct vpd_morph_data
	{
		std::u8string	    name{};
		Float32		weight{};
	};
	class vpd_post
	{
		std::u8string model_name_{};
		std::vector<vpd_bone_data> bones_;
		std::vector<vpd_morph_data> morphs_;
		CMT_DEFAULT_COPY_BODY(vpd_post)
		CMT_DEFAULT_MOVE_BODY(vpd_post)
	public:
		vpd_post(std::u8string model_name = {}) : model_name_(std::move(model_name)){}
		~vpd_post() = default;
		/* 用于从文件导入到对象 */
		bool load_from_file(const path& path);
		/* 用于将对象保存到文件 */
		bool save_to_file(const path& path) const;
	};
}