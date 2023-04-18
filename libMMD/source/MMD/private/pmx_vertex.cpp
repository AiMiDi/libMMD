/**************************************************************************

Copyright:Copyright(c) 2022-present, Aimidi & Walter White & CMT contributors.
Author:			Aimidi
Date:			2022/8/13
File:			pmx_vertex.cpp
Description:	pmx vertex data

**************************************************************************/

#include "pmx_vertex.h"

namespace libmmd
{
	bool PMXVertex::read_from_file(const file& file)
	{
		if (!file.read_elements(m_data->position))
			return false;
		if (!file.read_elements(m_data->normal))
			return false;
		if (!file.read_elements(m_data->UV))
			return false;
		if (!file.seek(16LL * model_info_->GetData()->additional_vec4_count))
			return false;
		if (!file.read_elements(&m_data->weight_deform_type))
			return false;
		m_data->weight_deform.reset(PMXWeight::alloc(m_data->weight_deform_type, model_info_));
		m_data->weight_deform->read_from_file(file);
		if (!file.read_elements(m_data->edge_scale))
			return false;
		return true;
	}
}