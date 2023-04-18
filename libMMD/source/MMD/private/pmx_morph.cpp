/**************************************************************************

Copyright:Copyright(c) 2022-present, Aimidi & Walter White & CMT contributors.
Author:			Aimidi
Date:			2022/9/18
File:			pmx_morph.cpp
Description:	pmx morph data

**************************************************************************/

#include "pmx_morph.h"

namespace libmmd
{
	bool PMXGroupMorphOffset::read_from_file(const file& file)
	{
		if (!model_info_->m_morph_index_reader(file, m_data->morph_index))
			return false;
		if (!file.read_elements(m_data->influence))
			return false;
		return true;
	}

	bool PMXVertexMorpOffset::read_from_file(const file& file)
	{
		if (!model_info_->m_vertex_index_reader(file, m_data->vertex_index))
			return false;
		if (!file.read_elements(m_data->position_offset))
			return false;
		return true;
	}

	bool PMXBoneMorphOffset::read_from_file(const file& file)
	{
		if (!model_info_->m_bone_index_reader(file, m_data->bone_index))
			return false;
		if (!file.read_elements(m_data->translation))
			return false;
		Vector4d32 quaternion_rotation;
		if (!file.read_elements(quaternion_rotation))
			return false;
		m_data->rotation = quaternion_to_euler(quaternion_rotation);
		return true;
	}

	bool PMXUVMorphOffset::read_from_file(const file& file)
	{
		if (!model_info_->m_vertex_index_reader(file, m_data->vertex_index))
			return false;
		if (!file.read_elements(m_data->UV_offset, sizeof m_data->UV_offset))
			return false;
		return true;
	}

	bool PMXMaterialMorphOffset::read_from_file(const file& file)
	{
		if (!model_info_->m_material_index_reader(file, m_data->material_index))
			return false;
		if (!file.read_elements(m_data->blend_mode))
			return false;
		if (!file.read_elements(m_data->diffuse))
			return false;
		if (!file.read_elements(m_data->specular))
			return false;
		if (!file.read_elements(m_data->specularity))
			return false;
		if (!file.read_elements(m_data->ambient))
			return false;
		if (!file.read_elements(m_data->edge_colour))
			return false;
		if (!file.read_elements(m_data->edge_size))
			return false;
		if (!file.read_elements(m_data->texture_tint))
			return false;
		if (!file.read_elements(m_data->environment_tint))
			return false;
		if (!file.read_elements(m_data->toon_tint))
			return false;
		return true;
	}

	bool PMXFlipMorphOffset::read_from_file(const file& file)
	{
		if (!model_info_->m_morph_index_reader(file, m_data->morph_index))
			return false;
		if (!file.read_elements(m_data->influence))
			return false;
		return true;
	}

	bool PMXImpulseMorphOffset::read_from_file(const file& file)
	{
		if (!model_info_->m_rigidbody_index_reader(file, m_data->rigid_body_index))
			return false;
		if (!file.read_elements(m_data->local_flag))
			return false;
		if (!file.read_elements(m_data->movement_speed))
			return false;
		if (!file.read_elements(m_data->rotation_torque))
			return false;
		return true;
	}

	bool PMXMorph::read_from_file(const file& file)
	{
		if (!model_info_->m_text_reader(file, m_data->morph_name_local))
			return false;
		if (!model_info_->m_text_reader(file, m_data->morph_name_universal))
			return false;
		if (!file.read_elements(m_data->panel_type))
			return false;
		if (!file.read_elements(m_data->morph_type))
			return false;
		if (!file.read_elements(m_data->offset_count))
			return false;
		m_data->offset_data.resize(m_data->offset_count);
		for (Int32 offset_index = 0; offset_index < m_data->offset_count; offset_index++)
		{
			auto& offset = m_data->offset_data[offset_index];
			switch (m_data->morph_type)
			{
			case PMXMorphData::GROUP:
			{
				offset = std::make_unique<PMXGroupMorphOffset>(model_info_);
				break;
			}
			case PMXMorphData::VERTEX:
			{
				offset = std::make_unique<PMXVertexMorpOffset>(model_info_);
				break;
			}
			case PMXMorphData::BONE:
			{
				offset = std::make_unique<PMXBoneMorphOffset>(model_info_);
				break;
			}
			case PMXMorphData::UV:
			case PMXMorphData::UV1:
			case PMXMorphData::UV2:
			case PMXMorphData::UV3:
			case PMXMorphData::UV4:
			{
				offset = std::make_unique<PMXUVMorphOffset>(model_info_, m_data->morph_type - 3);
				break;
			}
			case PMXMorphData::MATERIAL:
			{
				offset = std::make_unique<PMXMaterialMorphOffset>(model_info_);
				break;
			}
			case PMXMorphData::FLIP:
			{
				offset = std::make_unique<PMXFlipMorphOffset>(model_info_);
				break;
			}
			case PMXMorphData::IMPULSE:
			{
				offset = std::make_unique<PMXImpulseMorphOffset>(model_info_);
				break;
			}
			default:
				return false;
			}
			if (!offset->read_from_file(file))
				return false;
		}
		return true;
	}
}
