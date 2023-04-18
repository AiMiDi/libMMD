/**************************************************************************

Copyright:Copyright(c) 2022-present, Aimidi & Walter White & CMT contributors.
Author:			Aimidi
Date:			2022/8/9
File:			pmx_weight.cpp
Description:	pmx weight data

**************************************************************************/

#include "pmx_weight.h"

namespace libmmd
{
	PMXWeight* PMXWeight::alloc(const Char& type, const PMXModelInfo* index_reader)
	{
		switch (type)
		{
		case 0:
			return new PMXWeight_BDEF1(index_reader);
		case 1:
			return new PMXWeight_BDEF2(index_reader);
		case 2:
			return new PMXWeight_BDEF4(index_reader);
		case 3:
			return new PMXWeight_SDEF(index_reader);
		case 4:
			return new PMXWeight_QDEF(index_reader);
		default:
		{
			DebugStop("PMXWeight::Alloc type error!");
			return nullptr;
		}
		}
	}

	inline bool PMXWeight_BDEF1::read_from_file(const file& file)
	{
		if (!model_info_->m_bone_index_reader(file, m_data->bone))
			return false;
		return true;
	}
	inline bool PMXWeight_BDEF2::read_from_file(const file& file)
	{
		if (!model_info_->m_bone_index_reader(file, m_data->bone[0]))
			return false;
		if (!model_info_->m_bone_index_reader(file, m_data->bone[1]))
			return false;
		if (!file.read_elements(m_data->weight))
			return false;
		return true;
	}
	inline bool PMXWeight_BDEF4::read_from_file(const file& file)
	{
		if (!model_info_->m_bone_index_reader(file, m_data->bone[0]))
			return false;
		if (!model_info_->m_bone_index_reader(file, m_data->bone[1]))
			return false;
		if (!model_info_->m_bone_index_reader(file, m_data->bone[2]))
			return false;
		if (!model_info_->m_bone_index_reader(file, m_data->bone[3]))
			return false;
		if (!file.read_elements(m_data->weight[0]))
			return false;
		if (!file.read_elements(m_data->weight[1]))
			return false;
		if (!file.read_elements(m_data->weight[2]))
			return false;
		if (!file.read_elements(m_data->weight[3]))
			return false;
		return true;
	}
	inline bool PMXWeight_SDEF::read_from_file(const file& file)
	{
		if (!model_info_->m_bone_index_reader(file, m_data->bone[0]))
			return false;
		if (!model_info_->m_bone_index_reader(file, m_data->bone[1]))
			return false;
		if (!file.read_elements(m_data->weight))
			return false;
		if (!file.read_elements(m_data->R0))
			return false;
		if (!file.read_elements(m_data->R1))
			return false;
		if (!file.read_elements(m_data->C))
			return false;
		return true;
	}
	inline bool PMXWeight_QDEF::read_from_file(const file& file)
	{
		if (!model_info_->m_bone_index_reader(file, m_data->bone[0]))
			return false;
		if (!model_info_->m_bone_index_reader(file, m_data->bone[1]))
			return false;
		if (!model_info_->m_bone_index_reader(file, m_data->bone[2]))
			return false;
		if (!model_info_->m_bone_index_reader(file, m_data->bone[3]))
			return false;
		if (!file.read_elements(m_data->weight[0]))
			return false;
		if (!file.read_elements(m_data->weight[1]))
			return false;
		if (!file.read_elements(m_data->weight[2]))
			return false;
		if (!file.read_elements(m_data->weight[3]))
			return false;
		return true;
	}
}