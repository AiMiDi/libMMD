/**************************************************************************

Copyright:Copyright(c) 2022-present, Aimidi & Walter White & CMT contributors.
Author:			Aimidi
Date:			2022/8/13
File:			pmx_vertex.cpp
Description:	pmx vertex data

**************************************************************************/

#include "pmx_vertex.h"
#include "pmx_weight.h"

namespace libmmd
{
	 const std::array<float, 3>& pmx_vertex_impl::get_position() const
	{
		return position_;
	}

	 void pmx_vertex_impl::set_position(const std::array<float, 3>& position)
	{
		position_ = position;
	}

	 const std::array<float, 3>& pmx_vertex_impl::get_normal() const
	{
		return normal_;
	}

	 void pmx_vertex_impl::set_normal(const std::array<float, 3>& normal)
	{
		normal_ = normal;
	}

	 const std::array<float, 2>& pmx_vertex_impl::get_uv() const
	{
		return uv_;
	}

	 void pmx_vertex_impl::set_uv(const std::array<float, 2>& uv)
	{
		uv_ = uv;
	}

	 pmx_vertex_skinning::skinning_type pmx_vertex_impl::get_skinning_type() const
	{
		return skinning_type_;
	}

	 const pmx_vertex_skinning* pmx_vertex_impl::get_skinning() const
	{
		return skinning_.get();
	}

	pmx_vertex_skinning* pmx_vertex_impl::create_skinning(pmx_vertex_skinning::skinning_type skinning_type)
	{
		skinning_ = new_skinning(skinning_type_);
		return skinning_.get();
	}

	 float pmx_vertex_impl::get_edge_scale() const
	{
		return edge_scale_;
	}

	 void pmx_vertex_impl::set_edge_scale(const float scale)
	{
		edge_scale_ = scale;
	}

	bool pmx_vertex_impl::read_from_file(const file& file)
	{
		if (!file.read_element(position_))
			return false;
		if (!file.read_element(normal_))
			return false;
		if (!file.read_element(uv_))
			return false;
		if (!file.seek(16LL * model_description_->get_additional_vec4_count()))
			return false;
		if (!file.read_element(skinning_type_))
			return false;
		skinning_ = new_skinning(skinning_type_);

		// read skinning from file.
		switch (skinning_type_)
		{
		case pmx_vertex_skinning::skinning_type::NONE: [[unlikely]]
			break;
		case pmx_vertex_skinning::skinning_type::BDEF1:
			if (const auto skinning_ptr = std::dynamic_pointer_cast<pmx_vertex_skinning_BDEF1_impl>(skinning_); skinning_ptr) [[likely]]
			{
				skinning_ptr->read_from_file(file);
			}
			else [[unlikely]]
			{
				ELOGE << "pmx_vertex_impl::read_from_file: Can't cast skinning to BDEF1 skinning";
			}
			break;
		case pmx_vertex_skinning::skinning_type::BDEF2:
			if (const auto skinning_ptr = std::dynamic_pointer_cast<pmx_vertex_skinning_BDEF2_impl>(skinning_); skinning_ptr) [[likely]]
			{
				skinning_ptr->read_from_file(file);
			}
			else [[unlikely]]
			{
				ELOGE << "pmx_vertex_impl::read_from_file: Can't cast skinning to BDEF2 skinning";
			}
			break;
		case pmx_vertex_skinning::skinning_type::BDEF4:
			if (const auto skinning_ptr = std::dynamic_pointer_cast<pmx_vertex_skinning_BDEF4_impl>(skinning_); skinning_ptr) [[likely]]
			{
				skinning_ptr->read_from_file(file);
			}
			else [[unlikely]]
			{
				ELOGE << "pmx_vertex_impl::read_from_file: Can't cast skinning to BDEF4 skinning";
			}
			break;
		case pmx_vertex_skinning::skinning_type::SDEF:
			if (const auto skinning_ptr = std::dynamic_pointer_cast<pmx_vertex_skinning_SDEF_impl>(skinning_); skinning_ptr) [[likely]]
			{
				skinning_ptr->read_from_file(file);
			}
			else [[unlikely]]
			{
				ELOGE << "pmx_vertex_impl::read_from_file: Can't cast skinning to SDEF skinning";
			}
			break;
		case pmx_vertex_skinning::skinning_type::QDEF:
			if (const auto skinning_ptr = std::dynamic_pointer_cast<pmx_vertex_skinning_QDEF_impl>(skinning_); skinning_ptr) [[likely]]
			{
				skinning_ptr->read_from_file(file);
			}
			else [[unlikely]]
			{
				ELOGE << "pmx_vertex_impl::read_from_file: Can't cast skinning to QDEF skinning";
			}
			break;
		}

		if (!file.read_element(edge_scale_))
			return false;
		return true;
	}

	bool pmx_vertex_impl::write_to_file(const file& file) const
	{
		// TODO: write to file
		return false;
	}

	std::shared_ptr<pmx_vertex_skinning> pmx_vertex_impl::new_skinning(const pmx_vertex_skinning::skinning_type type) const
	{
		switch (type)
		{
		case pmx_vertex_skinning::skinning_type::NONE: [[unlikely]]
			break;
		case pmx_vertex_skinning::skinning_type::BDEF1:
			return std::make_shared<pmx_vertex_skinning_BDEF1_impl>(&model_description_->read_bone_index);
		case pmx_vertex_skinning::skinning_type::BDEF2: 
			return std::make_shared < pmx_vertex_skinning_BDEF2_impl>(&model_description_->read_bone_index);
		case pmx_vertex_skinning::skinning_type::BDEF4: 
			return std::make_shared < pmx_vertex_skinning_BDEF4_impl>(&model_description_->read_bone_index);
		case pmx_vertex_skinning::skinning_type::SDEF: 
			return std::make_shared < pmx_vertex_skinning_SDEF_impl>(&model_description_->read_bone_index);
		case pmx_vertex_skinning::skinning_type::QDEF: 
			return std::make_shared < pmx_vertex_skinning_QDEF_impl>(&model_description_->read_bone_index);
		}
		return {};
	}
}
