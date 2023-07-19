/**************************************************************************

Copyright:Copyright(c) 2022-present, Aimidi & Walter White & CMT contributors.
Author:			Aimidi
Date:			2023/6/4
File:			pmx_surface.h
Description:	pmx surface data

**************************************************************************/

#include "pmx_surface.h"

namespace libmmd
{
	inline const uint32_t& pmx_surface_impl::get_a() const
	{
		return index_[0];
	}

	inline void pmx_surface_impl::set_a(const uint32_t& point_index)
	{
		index_[0] = point_index;
	}

	inline const uint32_t& pmx_surface_impl::get_b() const
	{
		return index_[1];
	}

	inline void pmx_surface_impl::set_b(const uint32_t& point_index)
	{
		index_[1] = point_index;
	}

	inline const uint32_t& pmx_surface_impl::get_c() const
	{
		return index_[2];
	}

	inline void pmx_surface_impl::set_c(const uint32_t& point_index)
	{
		index_[2] = point_index;
	}

	bool pmx_surface_impl::read_from_file(const file& file)
	{
		if (!model_description_->read_vertex_index(file, index_[0]))
			return false;
		if (!model_description_->read_vertex_index(file, index_[1]))
			return false;
		if (!model_description_->read_vertex_index(file, index_[2]))
			return false;
		return true;
	}


	bool pmx_surface_impl::write_to_file(const file& file) const
	{
		// TODO: write to file
		return false;
	}
}
