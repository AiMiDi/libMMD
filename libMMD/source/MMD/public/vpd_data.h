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
	struct VPDBoneData
	{
		std::u8string      name{};
		Vector32	translate{};
		Vector4d32	rotation{};
	};
	struct VPDMorphData
	{
		std::u8string	    name{};
		Float32		weight{};
	};
	class VPDFile
	{
		std::u8string model_name{};
		std::vector<VPDBoneData> m_bones;
		std::vector<VPDMorphData> m_morphs;
		CMT_DISALLOW_COPY_AND_ASSIGN_BODY(VPDFile)
	public:
		VPDFile() = default;
		~VPDFile() = default;
		/* 用于从文件导入到对象 */
		bool load_from_file(const path& path);
		/* 用于将对象保存到文件 */
		bool save_to_file(const path& path) const;
	};
}