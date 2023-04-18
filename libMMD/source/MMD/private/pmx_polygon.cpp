/**************************************************************************

Copyright:Copyright(c) 2022-present, Aimidi & Walter White & CMT contributors.
Author:			Aimidi
Date:			2022/10/2
File:			pmx_polygon.h
Description:	pmx polygon data

**************************************************************************/

#include "pmx_polygon.h"

namespace libmmd
{
	bool PMXPolygon::read_from_file(const file& file)
	{
		if (!model_info_->m_vertex_index_reader(file, m_data->a))
			return false;
		if (!model_info_->m_vertex_index_reader(file, m_data->b))
			return false;
		if (!model_info_->m_vertex_index_reader(file, m_data->c))
			return false;
		return true;
	}
}
